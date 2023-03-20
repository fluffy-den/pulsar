/*! @file   concurrency_unit.cpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   10-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

// Include: Pulsar
#include "pulsar/pulsar.hpp"
#include "pulsar/allocator.hpp"
#include "pulsar/iterable.hpp"

// Include: Pulsar -> Tester
#include "pulsar_tester/pulsar_tester.hpp"

// Include: MoodyCamel
#include "concurrentqueue.h"

// Pulsar
namespace pul
{
	// MPSC Singly Lifo
	pt_pack(mpsc_singly_lifo_pack)
	{
		pt_benchmark(push_lifo_t1, __bvn, 65536, 1)
		{
			mpsc_singly_lifo<size_t> msl;
			mpsc_singly_lifo<size_t>::node_t *nodes = new_construct_array<mpsc_singly_lifo<size_t>::node_t>(65536);
			__bvn.measure([&](size_t __index)
			{
				return msl.insert_tail(&nodes[__index]);
			});
			destroy_delete_array(nodes);
		}
		pt_benchmark(push_lifo_t2, __bvn, 65536, 2)
		{
			mpsc_singly_lifo<size_t> msl;
			mpsc_singly_lifo<size_t>::node_t *nodes = new_construct_array<mpsc_singly_lifo<size_t>::node_t>(65536 * 2);
			__bvn.measure([&](size_t __index)
			{
				return msl.insert_tail(&nodes[__index]);
			});
			destroy_delete_array(nodes);
		}
		pt_benchmark(push_lifo_t4, __bvn, 65536, 4)
		{
			mpsc_singly_lifo<size_t> msl;
			mpsc_singly_lifo<size_t>::node_t *nodes = new_construct_array<mpsc_singly_lifo<size_t>::node_t>(65536 * 4);
			__bvn.measure([&](size_t __index)
			{
				return msl.insert_tail(&nodes[__index]);
			});
			destroy_delete_array(nodes);
		}
		pt_benchmark(push_lifo_t8, __bvn, 65536, 8)
		{
			mpsc_singly_lifo<size_t> msl;
			mpsc_singly_lifo<size_t>::node_t *nodes = new_construct_array<mpsc_singly_lifo<size_t>::node_t>(65536 * 8);
			__bvn.measure([&](size_t __index)
			{
				return msl.insert_tail(&nodes[__index]);
			});
			destroy_delete_array(nodes);
		}
		pt_benchmark(pop_empty_lifo_t8, __bvn, 65536, 8)
		{
			mpsc_singly_lifo<size_t> msl;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return msl.remove_head();
			});
		}
	}

	// MPMC Queue
	pt_pack(mpmc_queue_pack)
	{
		pt_benchmark(push_t1, __bvn, 65536, 1)
		{
			mpmc_queue<int32_t> queue(65536 * 2);

			int32_t *buf = new_construct_array<int32_t>(65536);
			for (size_t i = 0; i < 65536; ++i)
			{
				buf[i] = i;
			}

			__bvn.measure([&](size_t __index)
			{
				queue.enqueue(&buf[__index]);
				return __index;
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop_t1, __bvn, 262144, 1)
		{
			mpmc_queue<int32_t> queue(262144 * 2);

			int32_t *buf = new_construct_array<int32_t>(262144);
			for (size_t i = 0; i < 262144; ++i)
			{
				buf[i] = i;
				queue.enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop_t2, __bvn, 262144, 2)
		{
			mpmc_queue<int32_t> queue(262144 * 4);

			int32_t *buf = new_construct_array<int32_t>(262144 * 2);
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				buf[i] = i;
				queue.enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop_t4, __bvn, 262144, 4)
		{
			mpmc_queue<int32_t> queue(262144 * 8);

			int32_t *buf = new_construct_array<int32_t>(262144 * 4);
			for (size_t i = 0; i < 262144 * 4; ++i)
			{
				buf[i] = i;
				queue.enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop_t8, __bvn, 262144, 8)
		{
			mpmc_queue<int32_t> queue(262144 * 16);

			int32_t *buf = new_construct_array<int32_t>(262144 * 8 + 1);
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				buf[i] = i;
				queue.enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop_empty_t8, __bvn, 262144, 8)
		{
			mpmc_queue<int32_t> queue(262144 * 8);
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});
		}
	}

	// MPMC Queue2
	pt_pack(mpmc_queue2_pack)
	{
		pt_benchmark(push2_t1, __bvn, 262144, 1)
		{
			mpmc_queue2<int32_t> queue(262144 * 2);

			int32_t *buf = new_construct_array<int32_t>(262144);
			for (size_t i = 0; i < 262144; ++i)
			{
				buf[i] = i;
			}

			__bvn.measure([&](size_t __index)
			{
				queue.try_enqueue(&buf[__index]);
				return __index;
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(push2_t2, __bvn, 262144, 2)
		{
			mpmc_queue2<int32_t> queue(262144 * 4);

			int32_t *buf = new_construct_array<int32_t>(262144 * 2);
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				buf[i] = i;
			}

			__bvn.measure([&](size_t __index)
			{
				queue.try_enqueue(&buf[__index]);
				return __index;
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(push2_t4, __bvn, 262144, 4)
		{
			mpmc_queue2<int32_t> queue(262144 * 8);

			int32_t *buf = new_construct_array<int32_t>(262144 * 4);
			for (size_t i = 0; i < 262144 * 4; ++i)
			{
				buf[i] = i;
			}

			__bvn.measure([&](size_t __index)
			{
				queue.try_enqueue(&buf[__index]);
				return __index;
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(push2_t8, __bvn, 262144, 8)
		{
			mpmc_queue2<int32_t> queue(262144 * 16);

			int32_t *buf = new_construct_array<int32_t>(262144 * 8);
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				buf[i] = i;
			}

			__bvn.measure([&](size_t __index)
			{
				queue.try_enqueue(&buf[__index]);
				return __index;
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_t1, __bvn, 262144, 1)
		{
			mpmc_queue2<int32_t> queue(262144 * 2);

			int32_t *buf = new_construct_array<int32_t>(262144);
			for (size_t i = 0; i < 262144; ++i)
			{
				buf[i] = i;
				queue.try_enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_t2, __bvn, 262144, 2)
		{
			mpmc_queue2<int32_t> queue(262144 * 4);

			int32_t *buf = new_construct_array<int32_t>(262144 * 2);
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				buf[i] = i;
				queue.try_enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_t4, __bvn, 262144, 4)
		{
			mpmc_queue2<int32_t> queue(262144 * 8);

			int32_t *buf = new_construct_array<int32_t>(262144 * 4);
			for (size_t i = 0; i < 262144 * 4; ++i)
			{
				buf[i] = i;
				queue.try_enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_t8, __bvn, 262144, 8)
		{
			mpmc_queue2<int32_t> queue(262144 * 16);

			int32_t *buf = new_construct_array<int32_t>(262144 * 8 + 1);
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				buf[i] = i;
				queue.try_enqueue(&buf[i]);
			}

			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});

			destroy_delete_array(buf);
		}
		pt_benchmark(pop2_empty_t1, __bvn, 262144, 1)
		{
			mpmc_queue2<int32_t> queue(262144);
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});
		}
		pt_benchmark(pop2_empty_t8, __bvn, 262144, 8)
		{
			mpmc_queue2<int32_t> queue(262144 * 8);
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return queue.try_dequeue();
			});
		}
	}

	// MPSC Concurrent Queue
	pt_pack(moodycamel_queue)
	{
		pt_benchmark(moodycamel_push_t1, __bvn, 262144, 1)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			__bvn.measure([&](size_t __index)
			{
				return q.enqueue(__index);
			});
		}
		pt_benchmark(moodycamel_push_t2, __bvn, 262144, 2)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			__bvn.measure([&](size_t __index)
			{
				return q.enqueue(__index);
			});
		}
		pt_benchmark(moodycamel_push_t4, __bvn, 262144, 4)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			__bvn.measure([&](size_t __index)
			{
				return q.enqueue(__index);
			});
		}
		pt_benchmark(moodycamel_push_t8, __bvn, 262144, 8)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			__bvn.measure([&](size_t __index)
			{
				return q.enqueue(__index);
			});
		}
		pt_benchmark(moodycamel_pop_t1, __bvn, 262144, 1)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			for (size_t i = 0; i < 262144; ++i)
			{
				q.enqueue(i);
			}
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				size_t s;
				q.try_dequeue(s);
				return s;
			});
		}
		pt_benchmark(moodycamel_pop_t2, __bvn, 262144, 2)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				q.enqueue(i);
			}
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				size_t s;
				q.try_dequeue(s);
				return s;
			});
		}
		pt_benchmark(moodycamel_pop_t4, __bvn, 262144, 4)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			for (size_t i = 0; i < 262144 * 4; ++i)
			{
				q.enqueue(i);
			}
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				size_t s;
				q.try_dequeue(s);
				return s;
			});
		}
		pt_benchmark(moodycamel_pop_t8, __bvn, 262144, 8)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				q.enqueue(i);
			}
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				size_t s;
				q.try_dequeue(s);
				return s;
			});
		}
		pt_benchmark(moodycamel_pop_empty_t8, __bvn, 262144, 8)
		{
			moodycamel::ConcurrentQueue<size_t> q;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				size_t s;
				q.try_dequeue(s);
				return s;
			});
		}
	}

	// Atomic
	pt_pack(atomic_pack)
	{
		// Benchmarks
		pt_benchmark(zero_test_t1, __bvn, 262144, 1)
		{
			size_t k = 0;
			size_t t = 0;
			__bvn.measure([&](size_t __index)
			{
				auto s										= std::chrono::high_resolution_clock::now();
				auto b										= __rdtsc();
				pf_decl_volatile size_t i = __index + 16;
				auto e										= std::chrono::high_resolution_clock::now() - s;
				auto h										= __rdtsc() - b;
				k												 += e.count();
				t												 += h;
				return i;
			});
			const size_t tavg = t / 262144;
			const size_t kavg = k / 262144;
			pf_print("{} ticks = {} ns <=> 1 tick = {} ns\n", tavg, kavg, static_cast<float64_t>(kavg) / tavg);
		}
		pt_benchmark(atomic_load_t1, __bvn, 262144, 1)
		{
			pf_alignas(64) std::atomic<int32_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.load(atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_load_t2, __bvn, 262144, 2)
		{
			pf_alignas(64) std::atomic<int32_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.load(atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_load_t8, __bvn, 262144, 8)
		{
			pf_alignas(64) std::atomic<int32_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.load(atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_store_t1, __bvn, 262144, 1)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				k.store(__index, atomic_order::relaxed);
				return __index;
			});
		}
		pt_benchmark(atomic_store_t2, __bvn, 262144, 2)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				k.store(__index, atomic_order::relaxed);
				return __index;
			});
		}
		pt_benchmark(atomic_store_t8, __bvn, 262144, 8)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				k.store(__index, atomic_order::relaxed);
				return __index;
			});
		}
		pt_benchmark(atomic_fetch_add_t1, __bvn, 262144, 1)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.fetch_add(1, atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_fetch_add_t2, __bvn, 262144, 2)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.fetch_add(1, atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_fetch_add_t8, __bvn, 262144, 8)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore = __index;
				return k.fetch_add(1, atomic_order::relaxed);
			});
		}
		pt_benchmark(atomic_multi_cas_weak_t1, __bvn, 262144, 1)
		{
			pf_alignas(64) std::atomic<size_t> k = 0;
			__bvn.measure([&](size_t __index)
			{
				ignore	 = __index;
				size_t i = k.load(atomic_order::relaxed);
				k.compare_exchange_weak(i, i + 1, atomic_order::relaxed, atomic_order::relaxed);
				return i;
			});
		}
		pt_benchmark(atomic_multi_cas_weak_t2, __bvn, 262144, 2)
		{
			union
			{
				void *as_void;
				std::atomic<size_t> *as_size;
			} store;
			store.as_void = halloc(64 * 2, align_val_t(64), 0);
			__bvn.measure([&store](size_t __index)
			{
				ignore												= __index;
				pf_decl_thread_local size_t k = 0;
				size_t i											= k;
				do
				{
					size_t v = store.as_size[i].load(atomic_order::relaxed);
					if (store.as_size[i].compare_exchange_weak(v, v + 1, atomic_order::relaxed, atomic_order::relaxed))
					{
						k = i;
						return v;
					}
					++i;
				} while (true);
			});
			hfree(store.as_void);
		}
		pt_benchmark(atomic_multi_cas_weak_t8, __bvn, 262144, 8)
		{
			union
			{
				void *as_void;
				std::atomic<size_t> *as_size;
			} store;
			store.as_void = halloc(64 * 2, align_val_t(64), 0);
			__bvn.measure([&store](size_t __index)
			{
				ignore												= __index;
				pf_decl_thread_local size_t k = 0;
				size_t i											= k;
				do
				{
					size_t v = store.as_size[i].load(atomic_order::relaxed);
					if (store.as_size[i].compare_exchange_weak(v, v + 1, atomic_order::relaxed, atomic_order::relaxed))
					{
						k = i;
						return v;
					}
					++i;
				} while (true);
			});
			hfree(store.as_void);
		}
	}

	// SAMD Ring Allocator
	pt_pack(allocator_samd_ring_buffer_pack)
	{
		// Unit
		pt_unit(samd_ring_allocator_unicity)
		{
			allocator_samd_ring_buffer all(65536);
			for (size_t i = 0; i < 128; ++i)
			{
				void *p1 = all.allocate(4096);
				pt_require(p1);
				void *p2 = all.allocate(4096);
				pt_require(p2 && p2 != p1);
				void *p3 = all.allocate(4096);
				pt_require(p3 && p3 != p2);
				void *p4 = all.allocate(4096);
				pt_require(p4 && p4 != p3);
				all.deallocate(p1);
				all.deallocate(p2);
				all.deallocate(p3);
				all.deallocate(p4);
			}
		}

		// Benchmarks
		pt_benchmark(samd_ring_deal_t1, __bvn, 262144, 1)
		{
			allocator_samd_ring_buffer all(2ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 1 * sizeof(void*), align_val_t(8)));
			for (size_t i = 0; i < 262144 * 1; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return __index;
			});
			hfree(buf);
		}
		pt_benchmark(samd_ring_all_and_deal_t1, __bvn, 262144, 1)
		{
			allocator_samd_ring_buffer all(2ull * 262144 * (64 + 8));
			__bvn.measure([&](size_t __index)
			{
				ignore	= __index;
				void *a = all.allocate(64);
				all.deallocate(a);
				return a;
			});
		}
		pt_benchmark(samd_ring_deal_t2, __bvn, 262144, 2)
		{
			allocator_samd_ring_buffer all(4ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 2 * sizeof(void*), align_val_t(32)));
			for (size_t i = 0; i < 262144 * 2; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return buf[__index];
			});
			hfree(buf);
		}
		pt_benchmark(samd_ring_deal_t8, __bvn, 262144, 8)
		{
			allocator_samd_ring_buffer all(16ull * 262144 * (64 + 8));
			void **buf = union_cast<void**>(halloc(262144 * 8 * sizeof(void*), align_val_t(32)));
			for (size_t i = 0; i < 262144 * 8; ++i)
			{
				buf[i] = all.allocate(64);
			}
			__bvn.measure([&](size_t __index)
			{
				all.deallocate(buf[__index]);
				return buf[__index];
			});
			hfree(buf);
		}
		pt_benchmark(mimalloc_all_and_deal_t1, __bvn, 262144, 1)
		{
			__bvn.measure([&](size_t __index)
			{
				ignore	= __index;
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
		pt_benchmark(mimalloc_all_and_deal_t2, __bvn, 262144, 2)
		{
			__bvn.measure([&](size_t __index)
			{
				ignore	= __index;
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
		pt_benchmark(mimalloc_all_and_deal_t8, __bvn, 262144, 8)
		{
			__bvn.measure([&](size_t __index)
			{
				ignore	= __index;
				void *a = halloc(64);
				hfree(a);
				return a;
			});
		}
	}

}
