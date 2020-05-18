# ClothSimulation
基于质点弹簧模型的柔性布料仿真

![image](https://github.com/Firmament-cyou/ClothSimulation/blob/master/images/cloth2.PNG)

为了模拟真实的效果，opengl 窗口中加入了球体与地板的碰撞

WASD 与鼠标右键可以移动摄像机

为了提高性能，布料建模数据省略了法向量（故而没有加入布料的光照效果）

球体模型加入了 Phong 光照效果

弹簧力包括基于胡克定律的弹力与阻尼力，一定程度上减少了布料的抖动

场景中1234键分别松开布料的四个角，空格键还原初始状态

计算位移时使用四阶 Runge-Kutta 积分
