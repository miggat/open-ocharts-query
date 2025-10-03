#pragma once
#include <vector>
#include <memory>

// --- Clases mínimas de soporte geométrico para Osenc ---

// Triángulo primitivo (stub)
// class TriPrim {
// public:
//         TriPrim();
//         ~TriPrim();
//   void FreeMem(void);

//         unsigned int type;                  // Type of triangle primitive
//                                             //  May be PTG_TRIANGLES
//                                             //         PTG_TRIANGLE_STRIP
//                                             //         PTG_TRIANGLE_FAN

//         int         nVert;
//         double      *p_vertex;              //  Pointer to vertex array, x,y,x,y.....

//         LLBBox      tri_box;

//         TriPrim     *p_next;                // chain link
// };

// // Grupo de triángulos (stub)
// class PolyTriGroup {
// public:
//         PolyTriGroup();
//         PolyTriGroup(Extended_Geometry *pxGeom);
//         ~PolyTriGroup();

//         int             nContours;
//         int             *pn_vertex;             // pointer to array of poly vertex counts
//         float           *pgroup_geom;           // pointer to Raw geometry, used for contour line drawing

//         TriPrim         *tri_prim_head;         // head of linked list of TriPrims
//         bool            m_bSMSENC;
//         bool            bsingle_alloc;
//         unsigned char   *single_buffer;
//         int             single_buffer_size;
//         int             data_type;              //  p_vertex in TriPrim chain is FLOAT or DOUBLE
//         float           sfactor;
//         float           soffset;

//     private:
//         int my_bufgets( char *buf, int buf_len_max );

// };


// // Objeto de tesselation (stub)
// class PolyTessGeo {
// public:
//     PolyTessGeo() = default;
//     ~PolyTessGeo();

//     std::vector<std::unique_ptr<PolyTriGroup>> m_polytri_groups;
// };

// Geometría extendida (stub)
class Extended_Geometry {
public:
    Extended_Geometry() = default;
    ~Extended_Geometry() = default;

    std::vector<PolyTessGeo*> m_vec_polygeo;
};

