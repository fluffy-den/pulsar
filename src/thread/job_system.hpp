/*! @file   job_system.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   17-07-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.2
 */

// Include: Pulsar
#include "pulsar/job_system/job_system.hpp"

// Pulsar
namespace pul
{
	// Job System -> Internal Types
	struct __job_sys_worker;
	struct __job_sys_thread_storage;
	struct __job_sys_internal;

	// Job System -> Internal -> Thread Storage
	struct __job_sys_thread_storage
	{
		/// Constructors
		__job_sys_thread_storage() pf_attr_noexcept;
		__job_sys_thread_storage(
				size_t __wcs);

		/// Store
		thread_id_t id_;
		singly_cds_linked_lifo<job_system::__job> list_;
		allocator_ring allocator_;
	};

	// Job System -> Internal -> Worker
	struct __job_sys_worker
	{
		/// Process -> Handle Exception
		pf_decl_static void __proc_for_exception_task_handle(
				__job_sys_internal *__i,
				std::exception_ptr const &__e);

		/// Process -> Work on jobs
		pf_decl_static thread_id_t __proc(
				__job_sys_worker *__w,
				__job_sys_internal *__i,
				thread_id_t __id);

		/// Constructors
		__job_sys_worker();
		__job_sys_worker(size_t __wcs, __job_sys_internal *__i, thread_id_t __id);
		__job_sys_worker(
				__job_sys_worker const &) pf_attr_noexcept = delete;
		__job_sys_worker(
				__job_sys_worker &&) pf_attr_noexcept = delete;

		// This Thread (Workers)
		singly_node<__job_sys_thread_storage> store_;
		singly_rotative_linked_list<__job_sys_thread_storage>::iterator pos_;

		// Thread
		std::thread thread_;
	};

	// Job System -> Internal
	struct __job_sys_internal
	{
		pf_decl_friend class this_thread;

		/// Constructors
		__job_sys_internal();
		__job_sys_internal(__job_sys_internal const &) pf_attr_noexcept = delete;
		__job_sys_internal(__job_sys_internal &&) pf_attr_noexcept			= delete;

		/// Destructor
		~__job_sys_internal() pf_attr_noexcept;

		/// Process
		bool __process_list(
				singly_cds_linked_lifo<job_system::__job> &__l);
		bool __process_list_steal(
				singly_cds_linked_lifo<job_system::__job> &__l);
		void __process_0();
		void __process();

		/// Link
		void __link_and_notify(
				singly_cds_linked_lifo<job_system::__job> &__l,
				singly_node<job_system::__job> *__j) pf_attr_noexcept;
		void __link_0(
				singly_node<job_system::__job> *__j) pf_attr_noexcept;

		/// Job -> Unique Alloc
		singly_node<job_system::__job> *__create_unique_job(
				__job_sys_thread_storage *__store,
				std::initializer_list<job_task_initializer> const &__il);

		/// Job -> Unique With Future Alloc
		singly_node<job_system::__job> *__create_unique_with_future_job(
				__job_sys_thread_storage *__store,
				job_system::__future &__f,
				std::initializer_list<job_task_initializer> const &__il);

		/// Submit -> Unique
		void __submit(
				std::initializer_list<job_task_initializer> const &__il);
		void __submit_0(
				std::initializer_list<job_task_initializer> const &__il);

		/// Submit -> Unique with Future
		void __submit(
				job_system::__future &__f,
				std::initializer_list<job_task_initializer> const &__il);
		void __submit_0(
				job_system::__future &__f,
				std::initializer_list<job_task_initializer> const &__il);

		/// Submit -> Shared
		void __submit(
				singly_node<job_system::__job_shared> &__j,
				std::initializer_list<job_task_initializer> const &__il);
		void __submit_0(
				singly_node<job_system::__job_shared> &__j,
				std::initializer_list<job_task_initializer> const &__il);

		// Workers
		std::unique_ptr<singly_node<__job_sys_worker>[]> workers_;
		singly_rotative_linked_list<__job_sys_thread_storage> traveler_;

		// This Thread (Main)
		singly_cds_linked_lifo<job_system::__job> mainlist_;
		singly_node<__job_sys_thread_storage> store_;
		singly_rotative_linked_list<__job_sys_thread_storage>::iterator pos_;

		// Control
		byte_t cacheline1_[64];
		std::atomic_flag running_;
		byte_t cacheline2_[64];
		std::atomic<uint32_t> numJobs_;
		byte_t cacheline3_[64];

		// Static
		pf_decl_static pf_decl_inline std::unique_ptr<__job_sys_internal> instance_							 = nullptr;
		pf_decl_static pf_decl_inline pf_decl_thread_local __job_sys_thread_storage *localstore_ = nullptr;
	};
}