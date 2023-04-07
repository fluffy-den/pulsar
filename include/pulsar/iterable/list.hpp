/*! @file   list.hpp
 *  @author Louis-Quentin Noé (noe.louis-quentin@hotmail.fr)
 *  @brief
 *  @date   22-03-2023
 *
 *  @copyright Copyright (c) 2023 - Pulsar Software
 *
 *  @since 0.1.4
 */

#ifndef PULSAR_LIST_HPP
#define PULSAR_LIST_HPP 1

// Pulsar
namespace pul
{
	/*
	 * !        !
	 *   Singly
	 * !        !
	 */

	/// SINGLY: Node Concepts
	template <typename _NodeTy>
	concept __iterable_singly_node_c =
		requires(_NodeTy & __n)
	{
		{ __n.next }->std::same_as<_NodeTy*&>;
	};
	template <typename _NodeTy>
	struct is_singly_node : std::false_type
	{};
	template <__iterable_singly_node_c _NodeTy>
	struct is_singly_node<_NodeTy> : std::true_type
	{};
	template <typename _NodeTy>
	pf_decl_static pf_decl_constexpr bool is_singly_node_v = is_singly_node<_NodeTy>::value;

	/// SINGLY: Iterator
	template <typename _NodeTy>
	class singly_iterator
	{
	};
	template <typename _NodeTy>
	class singly_const_iterator
	{
	};

	/// SINGLY: View
	template <typename _NodeTy>
	class singly_view
	{
	};

	/// SINGLY: Sequenced -> List
	template <
		typename _NodeTy,
		typename _Magnifier,
		typename _Allocator>
	class singly_sequenced_list
	{

	};

	/// SINGLY: Sequenced -> Rotative List
	template <
		typename _NodeTy,
		typename _Magnifier,
		typename _Allocator>
	class singly_sequence_rotative_list
	{
	};

	/// SINGLY: List
	template <
		typename _NodeTy,
		typename _Allocator>
	class singly_list	// Unoptimized version
	{
	public:

	private:

	};

	/// SINGLY: Rotative List
	template <
		typename _NodeTy,
		typename _Allocator>
	class singly_rotative_list
	{
	public:

	private:
	};


	/*
	 * !        !
	 *   Doubly
	 * !        !
	 */

	/// DOUBLY: Node Concepts
	template <typename _NodeTy>
	concept __iterable_doubly_node_c =
		requires(_NodeTy & __n)
	{
		{ __n.next }->std::same_as<_NodeTy*&>;
		{ __n.prev }->std::same_as<_NodeTy*&>;
	};
	template <typename _NodeTy>
	struct is_doubly_node : std::false_type
	{};
	template <__iterable_doubly_node_c _NodeTy>
	struct is_doubly_node<_NodeTy> : std::true_type
	{};
	template <typename _NodeTy>
	pf_decl_static pf_decl_constexpr bool is_doubly_node_v = is_doubly_node<_NodeTy>::value;

	/// DOUBLY: Iterator
	template <typename _NodeTy>
	class doubly_iterator
	{
	};


	/// DOUBLY: View
	template <typename _NodeTy>
	class doubly_view
	{
	};

	/// DOUBLY: Sequenced -> List
	template <typename _NodeTy>
	class doubly_sequenced_list
	{
	};

	/// DOUBLY: Sequenced -> Rotative List
	template <typename _NodeTy>
	class doubly_sequenced_rotative_list
	{
	};

	/// DOUBLY: List
	template <typename _NodeTy>
	class doubly_list
	{
	};

	/// DOUBLY: Rotative List
	template <typename _NodeTy>
	class doubly_rotative_list
	{
	};
}

#endif // !PULSAR_LIST_HPP