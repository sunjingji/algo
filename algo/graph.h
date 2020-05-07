#pragma once

// ------------------------------------------------------------------------------
// 基础概念：
// ❑ 有向图、无向图
// ❑ 度、入度、出度
// ❑ 带权图
//
// 存储方式：
// ❑ 邻接矩阵
//    二维数组，简单，可以把运算转换为矩阵运算，但毕竟费内存，尤其是稀疏图的情形；
// ❑ 邻接表
//    哈希+链表的形式
//
// 图的搜索：
// ❑ 广度优先搜索(BFS)
//   广度优先搜索可以回答两类问题：A点到B点有没有路径，A点到B点哪条路径最短？
//   对边权重为1的无向图，广度优先搜索算法计算出来的两点之间的路径，就是两点的最短路径。
// ❑ 深度优先搜索(DFS)
//
// 最短路径(单源最短数据)：
// ❑ 贝尔曼-福特算法(Bellman-Ford)：
//    使用广度优先计算，依据每条边的权重，更新顶点的最短路径长度；当所有顶点的最短路径长度都不再变化，就找到了最短路径；
//    第i遍处理，可以保证从起点开始长度为i的最短路径，最多在n-1(顶点数-1)遍之后，最短路径就不会变化；如果在第n遍，最短路径依然在变化，则说明包含带负权重的回路；
//    可以处理带负边的有向图，但如果闭环中有负边则没有最短路径；带有负边的无向图没有最短路径，因为天然有回路；
//    算法复杂度O(m*n)
// ❑ 狄克斯特拉算法(Dijkstra)：
//    贪心算法思想，始终选择距起点最近的顶点(等同于确定了起点到该顶点的最短路径)，直到找到终点。Dijkstra算法不支持带负边的图，因为无法根据已知信息确定距起点最近的顶点，未知节点(可能带负权)可能会引入新的信息。
//    搜索最短路径是一个回溯穷举所有路径的过程，狄克斯算法实际是利用了动态规划思想(利用已经解决的顶点，推导后续节点的最短距离)，对回溯搜索进行了剪枝，只保留起点到某个顶点的最短路径，继续往外扩展搜索。
//    动态规划相较于回溯搜索，只是换了一个实现思路，但它实际上也考察到了所有从起点到终点的路线，所以才能得到最优解。
//    算法复杂度(m*logn)
// ❑ A*(A-Star)算法
//    A*（A-Star）算法也是一种在图中求解最短路径问题的算法，由狄克斯特拉算法发展而来。
//    狄克斯特拉算法会从离起点近的顶点开始，按顺序求出起点到各个顶点的最短路径。也就是说，一些离终点较远的顶点的最短路径也会被计算出来，但这部分其实是无用的。
//    如果我们知道每个顶点到终点距离的预估值，就可以启发算法优先选择更距离终点更近的顶点，减少对一些无用节点的搜索，提升效率。
//    狄克斯特拉算法对所有顶点一视同仁，加入启发信息后，估算距离近的节点被选择的可能性变大了。
//    通常使用“曼哈顿距离”(欧几里得距离需要开根号，计算太复杂)来估算顶点和终点的距离。曼哈顿距离是两点之间横纵坐标的距离(绝对值)之和。
//    启发式搜索算法利用估价函数，避免“跑偏”，贪心地朝着最有可能到达终点的方向前进。这种算法找出的路线，并不是最短路线。但是，实际的软件开发中的路线规划问题，我们往往并不需要非得找最短路线。
//    所以，鉴于启发式搜索算法能很好地平衡路线质量和执行效率，它在实际的软件开发中的应用更加广泛。
//    注意：
//      - Dijkstra算法是在终点出队列的时候才结束，A*算法是一旦遍历到终点就结束。对于Dijkstra算法来说，当终点出队列的时候，终点的dist值是优先级队列中所有顶点的最小值，即便再运行下去，终点的dist值也不会再被更新了。
//        对于A*算法来说，一旦遍历到终点，我们就结束 while 循环，这个时候，终点的 dist 值未必是最小值。
//      - 当估算距离小于实际距离时，一定可以得到正确答案(到达终点时，对所有剩余候选节点，如果估算距离小于已经找到的最短路径的实际距离，也需要再尝试一下，保证不漏掉可能的最短路径)；
//      - 但如果估算值和实际值误差较大，效率会变差。
// ❑ 小结
//    1)不存在负数权重时，更适合使用效率较高的狄克斯特拉算法。
//    2)而存在负数权重时，即便较为耗时，也应该使用可以得到正确答案的贝尔曼-福特算法。
//    3)像出行路线规划、游戏寻路，这些真实软件开发中的问题，一般情况下，我们都不需要非得求最优解（也就是最短路径）。在权衡路线规划质量和执行效率的情况下，我们只需要寻求一个次优解就足够了。这时候A*这种快速路径规划算法就非常理想了。
//
// 最小生成树：
//
// 二分图：
//
// 拓扑排序：
// 拓扑排序应用非常广泛，解决的问题的模型也非常一致：
//   1)凡是需要通过局部顺序来推导全局顺序的，一般都能用拓扑排序来解决。
//   2)除此之外，拓扑排序还能检测图中环的存在。
// 常用算法：
// ❑ Kahn算法：统计每个节点的入度，输出入度为零的节点并删除之，更新剩余节点的入度，重复这个删除/更新操作，直到节点数为零；
// ❑ DFS 算法：利用逆邻接表，使用深度优先遍历算法，递归处理每个顶点；