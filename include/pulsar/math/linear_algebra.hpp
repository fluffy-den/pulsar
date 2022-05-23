/*! @file   linear_algebra.hpp
 *  @author Fluffy (noe.louis-quentin@hotmail.fr)
 *  @brief  Defines mathematical objects to introduce the concepts of spaces.
 *  @date   16-05-2022
 *
 *  @copyright Copyright (c) 2022 - Pulsar Software
 *
 *  @since 0.1.1
 */

// Include: Pulsar
#include "pulsar/intrin.hpp"

// Include: C++
#include <array>
#include <type_traits>

// Pulsar
namespace pul
{
	// Mathematics
	namespace math
	{

		/// MATH: Matrix Definition
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		union matrix;

		/// MATH: Matrix -> Parameter Count
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		struct __matrix_parameter_count_of
		{
			pf_decl_static pf_decl_constexpr size_t value = 1;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 */
		template <
				typename _Ty,
				size_t _RowNum>
		struct __matrix_parameter_count_of<matrix<_Ty, _RowNum, 1>>
		{
			pf_decl_static pf_decl_constexpr size_t value = _RowNum;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _ColNum
		 */
		template <
				typename _Ty,
				size_t _ColNum>
		struct __matrix_parameter_count_of<matrix<_Ty, 1, _ColNum>>
		{
			pf_decl_static pf_decl_constexpr size_t value = _ColNum;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		pf_decl_static pf_decl_constexpr size_t __matrix_parameter_count_of_v =
				__matrix_parameter_count_of<_Ty>::value;

		/// MATH: Matrix -> Parameter Pack Count
		/*! @brief
		 *
		 *  @tparam _Args
		 */
		template <
				typename... _Args>
		struct __matrix_parameter_pack_count
		{
		public:
			pf_decl_static pf_decl_constexpr size_t value = {
				(__matrix_parameter_count_of_v<_Args> + ...)
			};
		};
		template <
				typename... _Args>
		pf_decl_static pf_decl_constexpr size_t __matrix_parameter_pack_count_v =
				__matrix_parameter_pack_count<_Args...>::value;

		/// MATH: Matrix -> Is Same Type
		/*! @brief
		 *
		 *  @tparam _TyA
		 *  @tparam _TyB
		 */
		template <
				typename _TyA,
				typename _TyB>
		struct __is_matrix_same
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = std::is_same_v<_TyA, _TyB>;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @tparam _ColNum
		 */
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		struct __is_matrix_same<_Ty, matrix<_Ty, _RowNum, _ColNum>>
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = true;
		};
		/*! @brief
		 *
		 *  @tparam _RowNum
		 *  @tparam _ColNum
		 *  @tparam _Ty
		 */
		template <
				size_t _RowNum,
				size_t _ColNum,
				typename _Ty>
		struct __is_matrix_same<matrix<_Ty, _RowNum, _ColNum>, _Ty>
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = true;
		};
		/*! @brief
		 *
		 *  @tparam _TyA
		 *  @tparam _TyB
		 */
		template <
				typename _TyA,
				typename _TyB>
		pf_decl_static pf_decl_constexpr bool __is_matrix_same_v =
				__is_matrix_same<_TyA, _TyB>::value;

		/// MATH: Matrix -> Is Row Vector
		/*! @brief
		 *
		 *  @tparam _MatrixTy
		 */
		template <
				typename _Ty>
		struct __is_matrix_col_vector
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = std::is_convertible_v<_Ty, matrix<_Ty, 1, 1>>;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 */
		template <
				typename _Ty,
				size_t _RowNum>
		struct __is_matrix_col_vector<matrix<_Ty, _RowNum, 1>>
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = true;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		pf_decl_static pf_decl_constexpr bool __is_matrix_col_vector_v =
				__is_matrix_col_vector<_Ty>::value;

		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		struct __is_matrix_row_vector
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = std::is_convertible_v<_Ty, matrix<_Ty, 1, 1>>;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 */
		template <
				typename _Ty,
				size_t _ColNum>
		struct __is_matrix_row_vector<matrix<_Ty, 1, _ColNum>>
		{
		public:
			pf_decl_static pf_decl_constexpr bool value = true;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		pf_decl_static pf_decl_constexpr bool __is_matrix_row_vector_v =
				__is_matrix_row_vector<_Ty>::value;

		/// MATH: Matrix -> Are Matrix Parameter Pack Row Vectors?
		/*! @brief
		 *
		 *  @tparam _Args
		 */
		template <
				typename... _Args>
		struct __are_matrix_parameter_pack_row_vectors
		{
		public:
			pf_decl_static pf_decl_constexpr bool value{
				(__is_matrix_row_vector_v<_Args> && ...)
			};
		};
		/*! @brief
		 *
		 *  @tparam _Args
		 */
		template <
				typename... _Args>
		pf_decl_static pf_decl_constexpr bool __are_matrix_parameter_pack_row_vectors_v =
				__are_matrix_parameter_pack_row_vectors<_Args...>::value;

		/// MATH: Matrix -> Are Matrix Parameter Pack Col Vectors?
		/*! @brief
		 *
		 *  @tparam _Args
		 */
		template <
				typename... _Args>
		struct __are_matrix_parameter_pack_col_vectors
		{
		public:
			pf_decl_static pf_decl_constexpr bool value{
				(__is_matrix_col_vector_v<_Args> && ...)
			};
		};
		/*! @brief
		 *
		 *  @tparam _Args
		 */
		template <
				typename... _Args>
		pf_decl_static pf_decl_constexpr bool __are_matrix_parameter_pack_col_vectors_v =
				__are_matrix_parameter_pack_col_vectors<_Args...>::value;

		/// MATH: Matrix -> Are Matrix Parameter Pack Same ?
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				typename... _Args>
		struct __are_matrix_parameter_pack_sames
		{
		public:
			pf_decl_static pf_decl_constexpr bool value{
				(__is_matrix_same_v<_Ty, _Args> && ...)
			};
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				typename... _Args>
		pf_decl_static pf_decl_constexpr bool __are_matrix_parameter_pack_sames_v =
				__are_matrix_parameter_pack_sames<_Ty, _Args...>::value;

		/// MATH: Matrix -> Is Row Vector Parameter Pack Correct ?
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				size_t _ColNum,
				typename... _Args>
		struct __is_row_vector_parameter_pack_correct
		{
		public:
			pf_decl_static pf_decl_constexpr bool value{
				__matrix_parameter_pack_count_v<_Args...> == _ColNum
				&& (__is_matrix_row_vector_v<_Args> && ...)
				&& (__is_matrix_same_v<_Ty, _Args> && ...)
			};
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				size_t _ColNum,
				typename... _Args>
		pf_decl_static pf_decl_constexpr bool __is_row_vector_parameter_pack_correct_v =
				__is_row_vector_parameter_pack_correct<_Ty, _ColNum, _Args...>::value;

		/// MATH: Matrix -> Is Col Vector Parameter Pack Correct ?
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _ColNum
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				size_t _RowNum,
				typename... _Args>
		struct __is_col_vector_parameter_pack_correct
		{
		public:
			pf_decl_static pf_decl_constexpr bool value{
				__matrix_parameter_pack_count_v<_Args...> == _RowNum
				&& (__is_matrix_col_vector_v<_Args> && ...)
				&& (__is_matrix_same_v<_Ty, _Args> && ...)
			};
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _ColNum
		 *  @tparam _Args
		 */
		template <
				typename _Ty,
				size_t _RowNum,
				typename... _Args>
		pf_decl_static pf_decl_constexpr bool __is_col_vector_parameter_pack_correct_v =
				__is_col_vector_parameter_pack_correct<_Ty, _RowNum, _Args...>::value;




		/// MATH: Matrix
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @tparam _ColNum
		 */
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		union alignas(simd_alignof(sizeof(_Ty), _ColNum)) matrix pf_attr_final
		{
		};

		// TODO: simd_alignof for booleans

		/// MATH: Row / Column Vector1
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _Align
		 */
		template <
				typename _Ty>
		union alignas(alignof(_Ty)) matrix<_Ty, 1, 1> pf_attr_final
		{
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 1> const &__r) pf_attr_noexcept
					: x(__r.x)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __val
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator=(
					_Ty __val) pf_attr_noexcept
			{
				this->x = __val;
				return *this;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->x;
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->x;
			}

			/// Operator+=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				return *this;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				return *this;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator~() pf_attr_noexcept
			{
				this->x = ~this->x;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<bool, 1, 1> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 1>(this->x == __val);
			}
			/*! @brief
			 *
			 */
			pf_decl_constexpr matrix<bool, 1, 1> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 1>(this->x <=> __val);
			}

			/// Operator _Ty
			/*! @brief
			 *
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr operator _Ty &() pf_attr_noexcept
			{
				return x;
			}
			/*! @brief
			 *
			 *  @return const _Ty
			 */
			pf_hint_nodiscard pf_decl_constexpr operator const _Ty &() const pf_attr_noexcept
			{
				return x;
			}

			_Ty x;
			_Ty r;

		private:
			std::array<_Ty, 1> data_;
		};
		/// MATH: Matrix -> Row Vector2
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 2)) matrix<_Ty, 1, 2> pf_attr_final
		{
		public:
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __val1
			 *  @param[in] __val2
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y) pf_attr_noexcept
					: x(__x)
					, y(__y)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->data_ = __r.data_;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator+=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator-=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator*=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x * __r.x + this->y * __r.y);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator*=(
					matrix<_Ty, 2, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this * __r[0],
						*this * __r[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator*=(
					matrix<_Ty, 2, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this * __r[0],
						*this * __r[1],
						*this * __r[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator*=(
					matrix<_Ty, 2, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this * __r[0],
						*this * __r[1],
						*this * __r[2],
						*this * __r[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator*=(
					matrix<_Ty, 2, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 1, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this * __r[i];
				}
				return as_matrix;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator/=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x / __r.x + this->y / __r.y);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator/=(
					matrix<_Ty, 2, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this / __r[0],
						*this / __r[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator/=(
					matrix<_Ty, 2, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this / __r[0],
						*this / __r[1],
						*this / __r[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator/=(
					matrix<_Ty, 2, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this / __r[0],
						*this / __r[1],
						*this / __r[2],
						*this / __r[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator/=(
					matrix<_Ty, 2, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 1, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this / __r[i];
				}
				return as_matrix;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				this->y %= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator%=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x %= __r.x;
				this->y %= __r.y;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator&=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator|=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator^=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				return *this;
			}
			/*! @brief
			 *
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator~() pf_attr_noexcept
			{
				this->x = ~this->x;
				this->y = ~this->y;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator>>=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> &operator<<=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 2> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 2>(
						this->x == __val,
						this->y == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 2> operator==(
					matrix<_Ty, 1, 2> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 2>(
						this->x == __r.x,
						this->y == __r.y);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 2> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 2>(
						this->x <=> __val,
						this->y <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 2> operator<=>(
					matrix<_Ty, 1, 2> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 2>(
						this->x <=> __r.x,
						this->y <=> __r.y);
			}

			struct
			{
				_Ty x;
				_Ty y;
			};
			struct
			{
				_Ty r;
				_Ty g;
			};

		private:
			std::array<_Ty, 2> data_;
		};

		/// MATH: Matrix -> Column Vector2
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 2)) matrix<_Ty, 2, 1> pf_attr_final
		{
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __val1
			 *  @param[in] __val2
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y) pf_attr_noexcept
					: x(__x)
					, y(__y)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->data_ = __r.data_;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator+=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator-=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 2> operator*=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 2>(
						*this * __r.x,
						*this * __r.y);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 3> operator*=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 3>(
						*this * __r.x,
						*this * __r.y,
						*this * __r.z);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 4> operator*=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 4>(
						*this * __r.x,
						*this * __r.y,
						*this * __r.z,
						*this * __r.w);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 2, _RightColNum> operator*=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 2, _RightColNum> as_matrix;
					matrix<_Ty, 2, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this * __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 2> operator/=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 2>(
						*this / __r.x,
						*this / __r.y);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 3> operator/=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 3>(
						*this / __r.x,
						*this / __r.y,
						*this / __r.z);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 2, 4> operator/=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 2, 4>(
						*this / __r.x,
						*this / __r.y,
						*this / __r.z,
						*this / __r.w);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 2, _RightColNum> operator/=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 2, _RightColNum> as_matrix;
					matrix<_Ty, 2, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this / __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator&=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator|=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator^=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator>>=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 2, 1> &operator<<=(
					matrix<_Ty, 2, 1> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 2, 1> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 2, 1>(
						this->x == __val,
						this->y == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 2, 1> operator==(
					matrix<_Ty, 2, 1> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 2, 1>(
						this->x == __r.x,
						this->y == __r.y);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 2, 1> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 2, 1>(
						this->x <=> __val,
						this->y <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 2, 1> operator<=>(
					matrix<_Ty, 2, 1> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 2, 1>(
						this->x <=> __r.x,
						this->y <=> __r.y);
			}

			struct
			{
				_Ty x;
				_Ty y;
			};
			struct
			{
				_Ty r;
				_Ty g;
			};

		private:
			std::array<_Ty, 2> data_;
		};

		/// MATH: Matrix -> Row Vector3
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 3)) matrix<_Ty, 1, 3> pf_attr_final
		{
		public:
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
					, z(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __y
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					_Ty __z) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__z)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 2> const &__r,
					_Ty __z) pf_attr_noexcept
					: x(__r.x)
					, y(__r.y)
					, z(__z)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 1, 2> const &__r)
					: x(__x)
					, y(__r.x)
					, z(__r.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x = __r.x;
				this->y = __r.y;
				this->z = __r.z;
				return *this;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				this->z += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator+=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				this->z -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator-=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator*=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x * __r.x + this->y * __r.y + this->z * __r.z);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator*=(
					matrix<_Ty, 3, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this * __r.data_[0],
						*this * __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator*=(
					matrix<_Ty, 3, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator*=(
					matrix<_Ty, 3, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2],
						*this * __r.data_[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator*=(
					matrix<_Ty, 3, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 1, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this * __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator/=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x / __r.x + this->y / __r.y + this->z / __r.z);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator/=(
					matrix<_Ty, 3, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this / __r.data_[0],
						*this / __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator/=(
					matrix<_Ty, 3, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this / __r[0],
						*this / __r[1],
						*this / __r[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator/=(
					matrix<_Ty, 3, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this / __r[0],
						*this / __r[1],
						*this / __r[2],
						*this / __r[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator/=(
					matrix<_Ty, 3, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 1, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this / __r[i];
				}
				return as_matrix;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				this->y %= __val;
				this->z %= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator%=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				this->z &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator&=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				this->z |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator|=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				this->z ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator^=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator~() pf_attr_noexcept
			{
				this->x = ~this->x;
				this->y = ~this->y;
				this->z = ~this->z;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				this->z >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator>>=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				this->z <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> &operator<<=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 3> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 3>(
						this->x == __val,
						this->y == __val,
						this->z == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 3> operator==(
					matrix<_Ty, 1, 3> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 3>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 3> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 3>(
						this->x <=> __val,
						this->y <=> __val,
						this->z <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 3> operator<=>(
					matrix<_Ty, 1, 3> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 3>(
						this->x <=> __r.x,
						this->y <=> __r.y,
						this->z <=> __r.z);
			}

			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
			};

		private:
			std::array<_Ty, 3> data_;
		};

		/// MATH: Matrix -> Column Vector3
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 3)) matrix<_Ty, 3, 1> pf_attr_final
		{
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
					, z(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __y
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					_Ty __z) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__z)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 2, 1> const &__r,
					_Ty __z) pf_attr_noexcept
					: x(__r.x)
					, y(__r.y)
					, z(__z)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 2, 1> const &__r)
					: x(__x)
					, y(__r.x)
					, z(__r.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x = __r.x;
				this->y = __r.y;
				this->z = __r.z;
				return *this;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				this->z += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator+=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				this->z -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator-=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 2> operator*=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 2>(
						*this * __r.data_[0],
						*this * __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 3> operator*=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 3>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 4> operator*=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 4>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2],
						*this * __r.data_[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 3, _RightColNum> operator*=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 3, _RightColNum> as_matrix;
					matrix<_Ty, 3, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[_RightColNum] = *this * __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 2> operator/=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 2>(
						*this / __r.data_[0],
						*this / __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 3> operator/=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 3>(
						*this / __r.data_[0],
						*this / __r.data_[1],
						*this / __r.data_[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 4> operator/=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 3, 4>(
						*this / __r.data_[0],
						*this / __r.data_[1],
						*this / __r.data_[2],
						*this / __r.data_[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 3, _RightColNum> operator/=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 3, _RightColNum> as_matrix;
					matrix<_Ty, 3, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[_RightColNum] = *this / __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				this->y %= __val;
				this->z %= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator%=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				this->z &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator&=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				this->z |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator|=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				this->z ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator^=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				this->z >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator>>=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				this->z <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 3, 1> &operator<<=(
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 3, 1> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 3, 1>(
						this->x == __val,
						this->y == __val,
						this->z == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 3, 1> operator==(
					matrix<_Ty, 3, 1> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 3, 1>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 3, 1> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 3, 1>(
						this->x <=> __val,
						this->y <=> __val,
						this->z <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 3, 1> operator<=>(
					matrix<_Ty, 3, 1> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 3, 1>(
						this->x <=> __r.x,
						this->y <=> __r.y,
						this->z <=> __r.z);
			}

			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
			};

		private:
			std::array<_Ty, 3> data_;
		};

		/// MATH: Matrix -> Row Vector4
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 4)) matrix<_Ty, 1, 4> pf_attr_final
		{
		public:
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
					, z(__val)
					, w(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __y
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					_Ty __z,
					_Ty __w) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__z)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 3> const &__r,
					_Ty __w) pf_attr_noexcept
					: x(__r.x)
					, y(__r.y)
					, z(__r.z)
					, w(__r.w)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
					: x(__x)
					, y(__r.x)
					, z(__r.y)
					, w(__r.z)
			{}
			/*! @brief
			 *
			 *  @param[in] __v1
			 *  @param[in] __v2
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 2> const &__v1,
					matrix<_Ty, 1, 2> const &__v2) pf_attr_noexcept
					: x(__v1.x)
					, y(__v1.y)
					, z(__v2.x)
					, w(__v2.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __v
			 *  @param[in] __w
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 1, 2> const &__v,
					_Ty __w) pf_attr_noexcept
					: x(__x)
					, y(__v.x)
					, z(__v.y)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __z
			 *  @param[in] __v
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					matrix<_Ty, 1, 2> const &__v) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__v.x)
					, w(__v.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __v
			 *  @param[in] __z
			 *  @param[in] __w
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 2> const &__v,
					_Ty __z,
					_Ty __w) pf_attr_noexcept
					: x(__v.x)
					, y(__v.y)
					, z(__z)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x = __r.x;
				this->y = __r.y;
				this->z = __r.z;
				this->w = __r.w;
				return *this;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				this->z += __val;
				this->w += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator+=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				this->w += __r.w;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				this->z -= __val;
				this->w -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator-=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				this->w -= __r.w;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				this->w *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator*=(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x * __r.x + this->y * __r.y + this->z * __r.z + this->w * __r.w);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator*=(
					matrix<_Ty, 4, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this * __r.data_[0],
						*this * __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator*=(
					matrix<_Ty, 4, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator*=(
					matrix<_Ty, 4, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this * __r.data_[0],
						*this * __r.data_[1],
						*this * __r.data_[2],
						*this * __r.data_[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator*=(
					matrix<_Ty, 4, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 4, _RightColNum> as_vector_list;
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this * __r;
				}
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				this->w /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 1> operator/=(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 1>(
						this->x / __r.x + this->y / __r.y + this->z / __r.z + this->w / __r.w);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 2> operator/=(
					matrix<_Ty, 4, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 2>(
						*this / __r.data_[0],
						*this / __r.data_[1]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 3> operator/=(
					matrix<_Ty, 4, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 3>(
						*this / __r.data_[0],
						*this / __r.data_[1],
						*this / __r.data_[2]);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> operator/=(
					matrix<_Ty, 4, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 1, 4>(
						*this / __r.data_[0],
						*this / __r.data_[1],
						*this / __r.data_[2],
						*this / __r.data_[3]);
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 1, _RightColNum> operator/=(
					matrix<_Ty, 4, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 1, _RightColNum> as_matrix;
					matrix<_Ty, 4, _RightColNum> as_vector_list;
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this / __r;
				}
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				this->y %= __val;
				this->z %= __val;
				this->w %= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator%=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				this->w %= __r.w;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				this->z &= __val;
				this->w &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator&=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				this->w &= __r.w;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				this->z |= __val;
				this->w |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator|=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				this->w |= __r.w;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				this->z ^= __val;
				this->w ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator^=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				this->w ^= __r.w;
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator~() pf_attr_noexcept
			{
				this->x = ~this->x;
				this->y = ~this->y;
				this->z = ~this->z;
				this->w = ~this->w;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				this->z <<= __val;
				this->w <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator<<=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				this->w <<= __r.w;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				this->z >>= __val;
				this->w >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator>>=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				this->w >>= __r.w;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x == __val,
						this->y == __val,
						this->z == __val,
						this->w == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator==(
					matrix<_Ty, 1, 4> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z,
						this->w == __r.w);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x <=> __val,
						this->y <=> __val,
						this->z <=> __val,
						this->w <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator<=>(
					matrix<_Ty, 1, 4> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x <=> __r.x,
						this->y <=> __r.y,
						this->z <=> __r.z,
						this->w <=> __r.w);
			}

			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
				_Ty w;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
				_Ty a;
			};

		private:
			std::array<_Ty, 4> data_;
		};

		/// MATH: Matrix -> Column Vector4
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <
				typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 4)) matrix<_Ty, 4, 1> pf_attr_final
		{
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0)) pf_attr_noexcept
					: x(__val)
					, y(__val)
					, z(__val)
					, w(__val)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __y
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					_Ty __z,
					_Ty __w) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__z)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @param[in] __z
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 3, 1> const &__r,
					_Ty __w) pf_attr_noexcept
					: x(__r.x)
					, y(__r.y)
					, z(__r.z)
					, w(__r.w)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 3, 1> const &__r) pf_attr_noexcept
					: x(__x)
					, y(__r.x)
					, z(__r.y)
					, w(__r.z)
			{}
			/*! @brief
			 *
			 *  @param[in] __v1
			 *  @param[in] __v2
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 2, 1> const &__v1,
					matrix<_Ty, 2, 1> const &__v2) pf_attr_noexcept
					: x(__v1.x)
					, y(__v1.y)
					, z(__v2.x)
					, w(__v2.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __v
			 *  @param[in] __w
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					matrix<_Ty, 2, 1> const &__v,
					_Ty __w) pf_attr_noexcept
					: x(__x)
					, y(__v.x)
					, z(__v.y)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __x
			 *  @param[in] __z
			 *  @param[in] __v
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __x,
					_Ty __y,
					matrix<_Ty, 2, 1> const &__v) pf_attr_noexcept
					: x(__x)
					, y(__y)
					, z(__v.x)
					, w(__v.y)
			{}
			/*! @brief
			 *
			 *  @param[in] __v
			 *  @param[in] __z
			 *  @param[in] __w
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 2, 1> const &__v,
					_Ty __z,
					_Ty __w) pf_attr_noexcept
					: x(__v.x)
					, y(__v.y)
					, z(__z)
					, w(__w)
			{}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator=(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
			{
				this->x = __r.x;
				this->y = __r.y;
				this->z = __r.z;
				this->w = __r.w;
				return *this;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i)
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) const
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*!
			 *  @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				this->x += __val;
				this->y += __val;
				this->z += __val;
				this->w += __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator+=(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
			{
				this->x += __r.x;
				this->y += __r.y;
				this->z += __r.z;
				this->w += __r.w;
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				this->x -= __val;
				this->y -= __val;
				this->z -= __val;
				this->w -= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator-=(
					matrix<_Ty, 4, 1> const &__r) pf_attr_noexcept
			{
				this->x -= __r.x;
				this->y -= __r.y;
				this->z -= __r.z;
				this->w -= __r.w;
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				this->x *= __val;
				this->y *= __val;
				this->z *= __val;
				this->w *= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 2> operator*=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 2>{
					*this * __r.data_[0],
					*this * __r.data_[1]
				};
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 3> operator*=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 3>{
					*this * __r.data_[0],
					*this * __r.data_[1],
					*this * __r.data_[2]
				};
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 4> operator*=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 4>{
					*this * __r.data_[0],
					*this * __r.data_[1],
					*this * __r.data_[2],
					*this * __r.data_[3]
				};
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _RightColNum
			 *  @param __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 4, _RightColNum> operator*=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 4, _RightColNum> as_matrix;
					matrix<_Ty, 4, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this * __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 1> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				this->x /= __val;
				this->y /= __val;
				this->z /= __val;
				this->w /= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 2> operator/=(
					matrix<_Ty, 1, 2> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 2>{
					*this / __r.data_[0],
					*this / __r.data_[1]
				};
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 3> operator/=(
					matrix<_Ty, 1, 3> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 3>{
					*this / __r.data_[0],
					*this / __r.data_[1],
					*this / __r.data_[2]
				};
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 4, 4> operator/=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				return matrix<_Ty, 4, 4>{
					*this / __r.data_[0],
					*this / __r.data_[1],
					*this / __r.data_[2],
					*this / __r.data_[3]
				};
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _RightColNum
			 *  @param __r
			 *  @return pf_decl_constexpr
			 */
			template <size_t _RightColNum>
			pf_decl_constexpr matrix<_Ty, 4, _RightColNum> operator/=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				union
				{
					matrix<_Ty, 4, _RightColNum> as_matrix;
					matrix<_Ty, 4, 1> as_vector_list[_RightColNum];
				};
				for (size_t i = 0; i < _RightColNum; ++i)
				{
					as_vector_list[i] = *this / __r.data_[i];
				}
				return as_matrix;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				this->x %= __val;
				this->y %= __val;
				this->z %= __val;
				this->w %= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator%=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x %= __r.x;
				this->y %= __r.y;
				this->z %= __r.z;
				this->w %= __r.w;
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				this->x &= __val;
				this->y &= __val;
				this->z &= __val;
				this->w &= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator&=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x &= __r.x;
				this->y &= __r.y;
				this->z &= __r.z;
				this->w &= __r.w;
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				this->x |= __val;
				this->y |= __val;
				this->z |= __val;
				this->w |= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator|=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x |= __r.x;
				this->y |= __r.y;
				this->z |= __r.z;
				this->w |= __r.w;
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				this->x ^= __val;
				this->y ^= __val;
				this->z ^= __val;
				this->w ^= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator^=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x ^= __r.x;
				this->y ^= __r.y;
				this->z ^= __r.z;
				this->w ^= __r.w;
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator~() pf_attr_noexcept
			{
				this->x = ~this->x;
				this->y = ~this->y;
				this->z = ~this->z;
				this->w = ~this->w;
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				this->x <<= __val;
				this->y <<= __val;
				this->z <<= __val;
				this->w <<= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator<<=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x <<= __r.x;
				this->y <<= __r.y;
				this->z <<= __r.z;
				this->w <<= __r.w;
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				this->x >>= __val;
				this->y >>= __val;
				this->z >>= __val;
				this->w >>= __val;
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, 4> &operator>>=(
					matrix<_Ty, 1, 4> const &__r) pf_attr_noexcept
			{
				this->x >>= __r.x;
				this->y >>= __r.y;
				this->z >>= __r.z;
				this->w >>= __r.w;
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator==(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x == __val,
						this->y == __val,
						this->z == __val,
						this->w == __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator==(
					matrix<_Ty, 1, 4> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x == __r.x,
						this->y == __r.y,
						this->z == __r.z,
						this->w == __r.w);
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator<=>(
					_Ty __val) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x <=> __val,
						this->y <=> __val,
						this->z <=> __val,
						this->w <=> __val);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, 4> operator<=>(
					matrix<_Ty, 1, 4> const &__r) const pf_attr_noexcept
			{
				return matrix<bool, 1, 4>(
						this->x <=> __r.x,
						this->y <=> __r.y,
						this->z <=> __r.z,
						this->w <=> __r.w);
			}

			struct
			{
				_Ty x;
				_Ty y;
				_Ty z;
				_Ty w;
			};
			struct
			{
				_Ty r;
				_Ty g;
				_Ty b;
				_Ty a;
			};

		private:
			std::array<_Ty, 4> data_;
		};

		/// MATH: Matrix -> Row Vector
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _ColNum
		 */
		template <
				typename _Ty,
				size_t _ColNum>
		union alignas(simd_alignof(sizeof(_Ty), _ColNum)) matrix<_Ty, 1, _ColNum> pf_attr_final
		{
		private:
			/*! @brief
			 *
			 *  @tparam _Args
			 *  @param[in] __val
			 *  @param[in] __i
			 *  @return pf_decl_constexpr
			 */
			template <
					typename... _Args>
			pf_decl_constexpr void __initialize_array_rec(
					_Ty __val,
					size_t &__i) pf_attr_noexcept
			{
				this->data_[__i] = __val;
				++__i;
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _InColNum
			 *  @tparam _Args
			 *  @param __vec
			 *  @param __i
			 *  @param __args
			 *  @return pf_decl_constexpr
			 */
			template <
					size_t _InColNum>
			pf_decl_constexpr void __initialize_array_rec(
					matrix<_Ty, 1, _InColNum> const &__vec,
					size_t &__i) pf_attr_noexcept
			{
				for (size_t i = 0; i < _InColNum; ++i)
				{
					this->data_[__i] = __vec[i];
					++__i;
				}
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _InTy
			 *  @tparam _Args
			 *  @param __i
			 *  @param __val
			 *  @param __args
			 *  @return pf_decl_constexpr
			 */
			template <
					typename _InTy,
					typename... _Args>
			pf_decl_constexpr void __initialize_array(
					size_t &__i,
					_InTy __val,
					_Args &&...__args) pf_attr_noexcept
			{
				__initialize_array_rec(__val, __i);
				if pf_decl_constexpr (sizeof...(_Args) > 0)
				{
					return __initialize_array(__i, __args...);
				}
			}

		public:
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0))
					: data_{ __val }
			{}
			/*! @brief
			 *
			 *  @tparam _Args
			 *  @param[in] __args
			 *  @return pf_decl_constexpr
			 */
			template <typename... _Args>
				requires(__is_row_vector_parameter_pack_correct_v<_Ty, _ColNum, _Args...>)
			pf_decl_constexpr matrix(
					_Args &&...__args)
			pf_attr_noexcept
			{
				size_t i = 0;
				this->__initialize_array(i, std::forward(__args)...);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				this->data_ = __r.data_;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i) pf_attr_noexcept
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) pf_attr_noexcept
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] += __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator+=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] += __r.data_[i];
				}
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] -= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator-=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] -= __r.data_[i];
				}
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] *= __r.data_[i];
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			template <
					size_t _RightColNum>
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _RightRowNum> &operator*=(
					matrix<_Ty, _ColNum, _RightColNum> const &__r) pf_attr_noexcept
			{
				matrix<_Ty, 1, _RightRowNum> tmp;
				for (size_t k = 0; k < _RightRowNum; ++k)
				{
					for (size_t i = 0; i < _ColNum; ++i)
					{
						tmp[k] = this->data_[i] * __r.data_[i];
					}
				}
				return tmp;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] /= __r.data_[i];
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			template <size_t _RightColNum>
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _RightRowNum> &operator/=(
					matrix<_Ty, _ColNum, _RightColNum> const &__r) pf_attr_noexcept
			{
				matrix<_Ty, 1, _RightRowNum> tmp;
				for (size_t k = 0; k < _RightRowNum; ++k)
				{
					for (size_t i = 0; i < _ColNum; ++i)
					{
						tmp[k] = this->data_[i] / __r.data_[i];
					}
				}
				return tmp;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator%=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __r.data_[i];
				}
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] &= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator&=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] &= __r.data_[i];
				}
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] |= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator|=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] |= __r.data_[i];
				}
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] ^= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator^=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] ^= __r.data_[i];
				}
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator~() pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] = ~this->data_[i];
				}
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] <<= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator<<=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] <<= __r.data_[i];
				}
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] >>= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, 1, _ColNum> &operator>>=(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] >>= __r.data_[i];
				}
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, _ColNum> operator==(
					_Ty __val) pf_attr_noexcept
			{
				matrix<bool, 1, _ColNum> tmp;
				for (size_t i = 0; i < _ColNum; ++i)
				{
					tmp[i] = this->data_[i] == __val;
				}
				return tmp;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, _ColNum> operator==(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				matrix<bool, 1, _ColNum> tmp;
				for (size_t i = 0; i < _ColNum; ++i)
				{
					tmp[i] = this->data_[i] == __r.data_[i];
				}
				return tmp;
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, _ColNum> operator<=>(
					_Ty __val) pf_attr_noexcept
			{
				matrix<bool, 1, _ColNum> tmp;
				for (size_t i = 0; i < _ColNum; ++i)
				{
					tmp[i] = this->data_[i] <=> __val;
				}
				return tmp;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<bool, 1, _ColNum> operator<=>(
					matrix<_Ty, 1, _ColNum> const &__r) pf_attr_noexcept
			{
				matrix<bool, 1, _ColNum> tmp;
				for (size_t i = 0; i < _ColNum; ++i)
				{
					tmp[i] = this->data_[i] <=> __r.data_[i];
				}
				return tmp;
			}

		private:
			std::array<_Ty, _ColNum> data_;
		};

		/// MATH: Matrix -> Column Vector
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 */
		template <typename _Ty, size_t _RowNum>
		union alignas(simd_alignof(sizeof(_Ty), _RowNum)) matrix<_Ty, _RowNum, 1> pf_attr_final
		{
		private:
			/*! @brief
			 *
			 *  @tparam _Args
			 *  @param[in] __val
			 *  @param[in] __i
			 *  @return pf_decl_constexpr
			 */
			template <
					typename... _Args>
			pf_decl_constexpr void __initialize_array_rec(
					_Ty __val,
					size_t &__i) pf_attr_noexcept
			{
				this->data_[__i] = __val;
				++__i;
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _InRowNum
			 *  @tparam _Args
			 *  @param __vec
			 *  @param __i
			 *  @param __args
			 *  @return pf_decl_constexpr
			 */
			template <
					size_t _InRowNum>
			pf_decl_constexpr void __initialize_array_rec(
					matrix<_Ty, _InRowNum, 1> const &__vec,
					size_t &__i) pf_attr_noexcept
			{
				for (size_t i = 0; i < _InRowNum; ++i)
				{
					this->data_[__i] = __vec[i];
					++__i;
				}
			}
			/*!
			 *  @brief
			 *
			 *  @tparam _InTy
			 *  @tparam _Args
			 *  @param __i
			 *  @param __val
			 *  @param __args
			 *  @return pf_decl_constexpr
			 */
			template <
					typename _InTy,
					typename... _Args>
			pf_decl_constexpr void __initialize_array(
					size_t &__i,
					_InTy __val,
					_Args &&...__args) pf_attr_noexcept
			{
				__initialize_array_rec(__val, __i);
				if pf_decl_constexpr (sizeof...(_Args) > 0)
				{
					return __initialize_array(__i, __args...);
				}
			}

		public:
			/// Constructors
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					_Ty __val = static_cast<_Ty>(0))
					: data_{ __val }
			{}
			/*! @brief
			 *
			 *  @tparam _Args
			 *  @param[in] __args
			 *  @return pf_decl_constexpr
			 */
			template <typename... _Args>
				requires(__is_col_vector_parameter_pack_correct_v<_Ty, _RowNum, _Args...>)
			pf_decl_constexpr matrix(
					_Args &&...__args)
			pf_attr_noexcept
			{
				size_t i = 0;
				this->__initialize_array(i, std::forward(__args)...);
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr
			 */
			pf_decl_constexpr matrix(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
					: data_(__r.data_)
			{}

			/// Operator=
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_decl_constexpr&
			 */
			pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				this->data_ = __r.data_;
			}

			/// Operator[]
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr _Ty &operator[](
					size_t __i) pf_attr_noexcept
			{
				return this->data_[__i];
			}
			/*! @brief
			 *
			 *  @param[in] __i
			 *  @return pf_hint_nodiscard const&
			 */
			pf_hint_nodiscard pf_decl_constexpr const _Ty &operator[](
					size_t __i) pf_attr_noexcept
			{
				return this->data_[__i];
			}

			/// Operator+=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator+=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] += __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator+=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] += __r.data_[i];
				}
				return *this;
			}

			/// Operator-=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator-=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] -= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator-=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] -= __r.data_[i];
				}
				return *this;
			}

			/// Operator*=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator*=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] *= __r.data_[i];
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			template <
					size_t _RightColNum>
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _RightColNum> operator*=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				matrix<_Ty, _RowNum, _RightColNum> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _RightColNum; ++j)
					{
						tmp[i][j] = this->data_[i] * __r.data_[j];
					}
				}
				return tmp;
			}

			/// Operator/=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator/=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] /= __r.data_[i];
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @tparam _RightColNum
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			template <
					size_t _RightColNum>
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _RightColNum> operator/=(
					matrix<_Ty, 1, _RightColNum> const &__r) pf_attr_noexcept
			{
				matrix<_Ty, _RowNum, _RightColNum> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					for (size_t j = 0; j < _RightColNum; ++j)
					{
						tmp[i][j] = this->data_[i] / __r.data_[j];
					}
				}
				return tmp;
			}

			/// Operator%=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator%=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator%=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __r.data_[i];
				}
				return *this;
			}

			/// Operator&=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator&=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] &= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator&=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] &= __r.data_[i];
				}
				return *this;
			}

			/// Operator|=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator|=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator|=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] %= __r.data_[i];
				}
				return *this;
			}

			/// Operator^=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator^=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] ^= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator^=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] ^= __r.data_[i];
				}
				return *this;
			}

			/// Operator~
			/*! @brief
			 *
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator~() pf_attr_noexcept
			{
				for (size_t i = 0; i < _RowNum; ++i)
				{
					this->data_[i] = ~this->data_[i];
				}
				return *this;
			}

			/// Operator<<=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator<<=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] <<= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator<<=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] <<= __r.data_[i];
				}
				return *this;
			}

			/// Operator>>=
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator>>=(
					_Ty __val) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] >>= __val;
				}
				return *this;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard&
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, 1> &operator>>=(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				for (size_t i = 0; i < _ColNum; ++i)
				{
					this->data_[i] >>= __r.data_[i];
				}
				return *this;
			}

			/// Operator==
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, 1> operator==(
					_Ty __val) pf_attr_noexcept
			{
				matrix<bool, _RowNum, 1> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					tmp[i] = this->data_[i] == __val;
				}
				return tmp;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, 1> operator==(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				matrix<bool, _RowNum, 1> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					tmp[i] = this->data_[i] == __r.data_[i];
				}
				return tmp;
			}

			/// Operator<=>
			/*! @brief
			 *
			 *  @param[in] __val
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, 1> operator<=>(
					_Ty __val) pf_attr_noexcept
			{
				matrix<bool, _RowNum, 1> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					tmp[i] = this->data_[i] <=> __val;
				}
				return tmp;
			}
			/*! @brief
			 *
			 *  @param[in] __r
			 *  @return pf_hint_nodiscard
			 */
			pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, 1> operator<=>(
					matrix<_Ty, _RowNum, 1> const &__r) pf_attr_noexcept
			{
				matrix<bool, _RowNum, 1> tmp;
				for (size_t i = 0; i < _RowNum; ++i)
				{
					tmp[i] = this->data_[i] <=> __r.data_[i];
				}
				return tmp;
			}

		private:
			std::array<_Ty, _RowNum> data_;
		};

		/// MATH: Matrix
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 2)) matrix<_Ty, 2, 2> pf_attr_final
		{
		public:
			/// Constructors

			/// Operator=

			// TODO

		private:
			std::array<matrix<_Ty, 1, 2>, 2> data_;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 3)) matrix<_Ty, 3, 3> pf_attr_final
		{
		private:
			std::array<matrix<_Ty, 1, 3>, 3> data_;
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 4)) matrix<_Ty, 4, 4> pf_attr_final
		{
		private:
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 2)) matrix<_Ty, 3, 2> pf_attr_final
		{
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 3)) matrix<_Ty, 2, 3> pf_attr_final
		{
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 2)) matrix<_Ty, 4, 2> pf_attr_final
		{
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 3)) matrix<_Ty, 4, 3> pf_attr_final
		{
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 4)) matrix<_Ty, 2, 4> pf_attr_final
		{
		};
		/*! @brief
		 *
		 *  @tparam _Ty
		 */
		template <typename _Ty>
		union alignas(simd_alignof(sizeof(_Ty), 4)) matrix<_Ty, 3, 4> pf_attr_final
		{
		};

		/// MATH: Matrix: Operator+
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator+(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp += __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator+(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp += __r;
		}

		/// MATH: Matrix: Operator-
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator-(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp -= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator-(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp -= __r;
		}


		/// MATH: Matrix: Operator*
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _LeftRowNum
		 *  @tparam _LeftColNum
		 *  @param[in] __l
		 *  @param[in] __val
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _LeftRowNum,
				size_t _LeftColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _LeftRowNum, _LeftColNum> operator*(
				matrix<_Ty, _LeftRowNum, _LeftColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _LeftRowNum, _LeftColNum> tmp(__l);
			return tmp *= __val;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _LeftRowNum
		 *  @tparam _LeftColNum
		 *  @tparam _RightColNum
		 *  @param[in] __l
		 *  @param[in] __r
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _LeftRowNum,
				size_t _LeftColNum,
				size_t _RightColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _LeftRowNum, _RightColNum> operator*(
				matrix<_Ty, _LeftRowNum, _LeftColNum> const &__l,
				matrix<_Ty, _LeftColNum, _RightColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LeftRowNum, _LeftColNum> tmp(__l);
			return tmp *= __r;
		}

		/// MATH: Matrix: Operator/
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _LeftRowNum
		 *  @tparam _LeftColNum
		 *  @param[in] __l
		 *  @param[in] __val
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _LeftRowNum,
				size_t _LeftColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _LeftRowNum, _LeftColNum> operator/(
				matrix<_Ty, _LeftRowNum, _LeftColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _LeftRowNum, _LeftColNum> tmp(__l);
			return tmp /= __val;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _LeftRowNum
		 *  @tparam _LeftColNum
		 *  @tparam _RightColNum
		 *  @param[in] __l
		 *  @param[in] __r
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _LeftRowNum,
				size_t _LeftColNum,
				size_t _RightColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _LeftColNum, _RightColNum> operator/(
				matrix<_Ty, _LeftRowNum, _LeftColNum> const &__l,
				matrix<_Ty, _LeftColNum, _RightColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _LeftRowNum, _LeftColNum> tmp(__l);
			return tmp /= __r;
		}

		/// MATH: Matrix: Operator%
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator%(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp %= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator%(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp %= __r;
		}

		/// MATH: Matrix: Operator&
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator&(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp &= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator&(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp &= __r;
		}

		/// MATH: Matrix: Operator|
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator|(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp |= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator|(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp |= __r;
		}

		/// MATH: Matrix: Operator^
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator^(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp ^= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator^(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp ^= __r;
		}

		/// MATH: Matrix: Operator>>
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator>>(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp >>= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator>>(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp >>= __r;
		}

		/// MATH: Matrix: Operator<<
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator<<(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp <<= __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _RowNum, _ColNum> operator<<(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp <<= __r;
		}

		/// MATH: Matrix: Operator==
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum> operator==(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp == __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum> operator==(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp == __r;
		}

		/// MATH: Matrix: Operator<=>
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum> operator<=>(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				_Ty __val) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp <=> __val;
		}
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<bool, _RowNum, _ColNum> operator<=>(
				matrix<_Ty, _RowNum, _ColNum> const &__l,
				matrix<_Ty, _RowNum, _ColNum> const &__r) pf_attr_noexcept
		{
			matrix<_Ty, _RowNum, _ColNum> tmp(__l);
			return tmp <=> __r;
		}

		/// MATH: Functions
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _ColNum
		 *  @param[in] __matrix
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _RowNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, 1, _RowNum> transpose(
				matrix<_Ty, _RowNum, 1> const &__matrix) pf_attr_noexcept
		{
			matrix<_Ty, 1, _RowNum> tmp;
			for (size_t i = 0; i < _RowNum; ++i)
			{
				tmp[i] = __matrix[i];
			}
			return tmp;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @param[in] __matrix
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _ColNum, 1> transpose(
				matrix<_Ty, 1, _ColNum> const &__matrix) pf_attr_noexcept
		{
			matrix<_Ty, _ColNum, 1> tmp;
			for (size_t i = 0; i < _ColNum; ++i)
			{
				tmp[i] = __matrix[i];
			}
			return tmp;
		}
		/*! @brief
		 *
		 *  @tparam _Ty
		 *  @tparam _RowNum
		 *  @tparam _ColNum
		 *  @param[in] __matrix
		 *  @return pf_hint_nodiscard
		 */
		template <
				typename _Ty,
				size_t _RowNum,
				size_t _ColNum>
		pf_hint_nodiscard pf_decl_constexpr matrix<_Ty, _ColNum, _RowNum> transpose(
				matrix<_Ty, _RowNum, _ColNum> const &__matrix) pf_attr_noexcept
		{
			matrix<_Ty, _ColNum, _RowNum> tmp;
			for (size_t i = 0; i < _ColNum; ++i)
			{
				for (size_t j = 0; j < _RowNum; ++j)
				{
					tmp[i][j] = __matrix[j][i];
				}
			}
			return tmp;
		}

		// TODO: Determinant

		// TODO: Inverse

		/// MATH: Vector
		template <typename _Ty>
		using vector1 = matrix<_Ty, 1, 1>;
		template <typename _Ty>
		using vector2 = matrix<_Ty, 1, 2>;
		template <typename _Ty>
		using vector3 = matrix<_Ty, 1, 3>;
		template <typename _Ty>
		using vector4 = matrix<_Ty, 1, 4>;

		/// MATH: Matrix AxB
		template <typename _Ty>
		using matrix1x1 = matrix<_Ty, 1, 1>;
		template <typename _Ty>
		using matrix1x2 = matrix<_Ty, 1, 2>;
		template <typename _Ty>
		using matrix2x1 = matrix<_Ty, 2, 1>;
		template <typename _Ty>
		using matrix2x2 = matrix<_Ty, 2, 2>;
		template <typename _Ty>
		using matrix3x1 = matrix<_Ty, 3, 1>;
		template <typename _Ty>
		using matrix3x2 = matrix<_Ty, 3, 2>;
		template <typename _Ty>
		using matrix1x3 = matrix<_Ty, 1, 3>;
		template <typename _Ty>
		using matrix2x3 = matrix<_Ty, 2, 3>;
		template <typename _Ty>
		using matrix3x3 = matrix<_Ty, 3, 3>;
		template <typename _Ty>
		using matrix4x1 = matrix<_Ty, 4, 1>;
		template <typename _Ty>
		using matrix4x2 = matrix<_Ty, 4, 2>;
		template <typename _Ty>
		using matrix4x3 = matrix<_Ty, 4, 3>;
		template <typename _Ty>
		using matrix1x4 = matrix<_Ty, 1, 4>;
		template <typename _Ty>
		using matrix2x4 = matrix<_Ty, 2, 4>;
		template <typename _Ty>
		using matrix3x4 = matrix<_Ty, 3, 4>;
		template <typename _Ty>
		using matrix4x4 = matrix<_Ty, 4, 4>;

		/// MATH: Quaternion
		// TODO: Math Quaternions
	}
}