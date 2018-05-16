#pragma once
struct matrix_tag {
};
struct vector_tag {
};

template<typename MV>
struct matrix_vector_traits {
    typedef typename MV::tag tag;
};

template<typename MV1, typename MV2,
        typename tag1, typename tag2>
struct return_type_of {
    typedef MV1 type;
};

template<typename MV1, typename MV2>
struct return_type_of<MV1,MV2,matrix_tag,vector_tag> {
    typedef MV2 type;
};

template <typename MV1, typename MV2>
typename struct return_type_of<MV1,MV2, typename matrix_vector_traits<MV1>::tag,
        typename matrix_vector_traits<MV2>::tag>::type
prod(MV1 const&mv1, MV2 const&mv2){
    return prod(mv1,mv2,typename matrix_vector_traits<MV1>::tag(),typename matrix_vector_traits<MV2>::tag());
};
// 特化
template <typename Matrix1, typename Matrix2>
Matrix1 prod(Matrix1 const &mv1, Matrix2 const&mv2, matrix_tag, matrix_tag);

template <typename Matrix, typename Vector>
Vector prod(Matrix const &mv1, Vector const&mv2, matrix_tag, vector_tag);

template <typename Vector, typename Matrix>
Vector prod(Vector const &mv1, Matrix const&mv2, vector_tag, matrix_tag);