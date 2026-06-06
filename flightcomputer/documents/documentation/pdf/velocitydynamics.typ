
// $
// bold(q)bold(a) = vec(
//     q_1 a_0 - q_2 a_x - q_3 a_y - q_4 a_z,
//     q_1 a_x + q_2 a_0 + q_3 a_z - q_4 a_y,
//     q_1 a_y - q_2 a_z + q_3 a_0 + q_4 a_x,
//     q_1 a_z + q_2 a_y - q_3 a_x + q_4 a_0,
// )
// $
// #figure(
// table(
//   stroke: none,
//   gutter: 0.2em,
//   inset: (right: 1.5em),
//   columns: 2,
//   table.header[ $q_1=$ ][ $q_2=$ ],
//   [$+(q_1 a_0 - q_2 a_x - q_3 a_y - q_4 a_z)q_1 \
// +(q_1 a_x + q_2 a_0 + q_3 a_z - q_4 a_y)q_2 \
// +(q_1 a_y - q_2 a_z + q_3 a_0 + q_4 a_x)q_3 \
// +(q_1 a_z + q_2 a_y - q_3 a_x + q_4 a_0)q_4$],
//   [$-(q_1 a_0 - q_2 a_x - q_3 a_y - q_4 a_z)q_2 \
// +(q_1 a_x + q_2 a_0 + q_3 a_z - q_4 a_y)q_1 \
// -(q_1 a_y - q_2 a_z + q_3 a_0 + q_4 a_x)q_4 \
// +(q_1 a_z + q_2 a_y - q_3 a_x + q_4 a_0)q_3$],
// [ $q_3=$ ],[ $q_4=$ ],
//   [$-(q_1 a_0 - q_2 a_x - q_3 a_y - q_4 a_z)q_3 \
// +(q_1 a_x + q_2 a_0 + q_3 a_z - q_4 a_y)q_4 \
// +(q_1 a_y - q_2 a_z + q_3 a_0 + q_4 a_x)q_1 \
// -(q_1 a_z + q_2 a_y - q_3 a_x + q_4 a_0)q_2$],
//   [$-(q_1 a_0 - q_2 a_x - q_3 a_y - q_4 a_z)q_4 \
// -(q_1 a_x + q_2 a_0 + q_3 a_z - q_4 a_y)q_3 \
// +(q_1 a_y - q_2 a_z + q_3 a_0 + q_4 a_x)q_2 \
// +(q_1 a_z + q_2 a_y - q_3 a_x + q_4 a_0)q_1$]
// ))
$
  bold(F)_(v q) = mat(
    2(q_1a_x+q_3a_z-q_4a_y)dif t,2(q_2a_x+q_3a_y+q_4a_z)dif t,2(q_1a_z-q_3a_x+q_2a_y)dif t,2(q_4a_x-q_1a_y+q_2a_z)dif t;
    2(q_1a_y+q_4a_x-q_2a_z)dif t,2(q_3a_x-q_2a_y-q_1a_z)dif t,2(q_3a_y+q_2a_x+q_4a_z)dif t,2(q_3a_z-q_4a_y+q_1a_x)dif t;
    2(q_1a_z+q_2a_y-q_3a_x)dif t,2(q_4a_x-q_2a_z+q_1a_y)dif t,2(q_4a_y-q_3a_z-q_1a_x)dif t,2(q_4a_z+q_2a_x+q_3a_y)dif t;
  )
$
$
bold(F)_(v v) = mat(1,0,0;0,1,0;0,0,1)
$
