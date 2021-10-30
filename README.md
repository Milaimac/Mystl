# Mystl
try to implement the core of stl

---
## vector.hpp

构建思路
- 原本是写一个简单的vector,后面写来写去发现满足不了普通的需求
- 本来是想直接使用 new 直接配置空间并且初始化， 但是这不符合实际的 start - finish - endofstrore
- 最终还是使用malloc先是配置了空间， 再就是使用placement new将之初始化
- 最后写的时候，也懂了vectorbase的作用， 能让我少写一点重复的构造函数
- 最核心的insert_aux其中的内存扩充， 将原来vector内容销毁 ，转移到新的vector

难点
- 没有construct 和 uninitialized_fill 等函数的帮助， 在配置空空间并初始化的步骤要自己写



