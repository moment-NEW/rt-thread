# 这是草稿
WS2812的驱动时序要求比较严格，大概如下：
脉冲	定义	时间	误差
T0H	0 code ,high voltage time	0.4us	±150ns
T1H	1 code ,high voltage time	0.85us	±150ns
T0L	0 code , low voltage time	0.85us	±150ns
T1L	1 code ,low voltage time	0.4us	±150ns
RES	low voltage time	Above 50μs


也就是我们要做到发送0和1两种帧，其占空比有很大的要求，所以需要调整SPI来模拟
而我们配置的BSP,核心频率200Mhz（没开满的），
SPI 时钟为 
𝑓，每个 SPI bit 的时间为：
T_{SPI} = \frac{1}{f}
因此