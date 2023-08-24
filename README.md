将文件下载至本地后使用obsidian观看体验更佳，因为链接的语法格式和一般markdown不同，而obsidian良好的自动识别并跳转。
# 目录
- [[#1. 背景|1. 背景]]
- [[#2. 项目基础|2. 项目基础]]
- [[#3. 项目技术路线|3. 项目技术路线]]
- [[#4. Leap Motion开发与手势信息提取|4. Leap Motion开发与手势信息提取]]
	- [[#4. Leap Motion开发与手势信息提取#4.1 Leap Motion介绍|4.1 Leap Motion介绍]]
	- [[#4. Leap Motion开发与手势信息提取#4.2 驱动程序|4.2 驱动程序]]
	- [[#4. Leap Motion开发与手势信息提取#4.3 开发环境|4.3 开发环境]]
	- [[#4. Leap Motion开发与手势信息提取#4.4 手势信息|4.4 手势信息]]
- [[#5. 远程通信|5. 远程通信]]
	- [[#5. 远程通信#5.1 串口蓝牙模块|5.1 串口蓝牙模块]]
		- [[#5.1 串口蓝牙模块#5.1.1 模块与单边机的连接|5.1.1 模块与单边机的连接]]
		- [[#5.1 串口蓝牙模块#5.1.2 模块与电脑的连接|5.1.2 模块与电脑的连接]]
		- [[#5.1 串口蓝牙模块#5.1.3 蓝牙模块AT指令设置|5.1.3 蓝牙模块AT指令设置]]
	- [[#5. 远程通信#5.2 数据传输|5.2 数据传输]]
		- [[#5.2 数据传输#5.2.1 数据类型转换|5.2.1 数据类型转换]]
		- [[#5.2 数据传输#5.2.2 数据传输与接收方式|5.2.2 数据传输与接收方式]]
		- [[#5.2 数据传输#5.2.3 传输速率|5.2.3 传输速率]]
- [[#6. 控制策略|6. 控制策略]]
- [[#7. 项目成果|7. 项目成果]]
- [[#8. 总结展望|8. 总结展望]]
- [[#9. 其他参考资料|9. 其他参考资料]]

## 1. 背景
假人是一种用来模拟车祸对人体所造成的影响的仪器，它们装有传感器，可以测量碰撞中各部位的损伤情况。假人有不同的类型，分别模拟不同年龄、性别和身材的人体。假人的主动响应是指它们可以根据碰撞场景和车辆的安全系统，采取一定的姿态和动作，以减少伤害或提高测试精度。例如，有些假人可以模拟紧急刹车时的前倾动作，或者模拟行人避让车辆时的行走轨迹。假人的主动响应是汽车安全技术发展的重要方面，它可以帮助评估和改进车辆的主动安全系统，如自动紧急制动（AEB）系统。本项目主要内容是利用能够根据图像捕捉手部信息的Leap Motion，无接触式地远程控制仿生机械手实现假人在碰撞发生时的手部动作。
## 2. 项目基础
单位目前已实现通过佩戴绑定柔性传感器的手套来控制机械手的功能，已具备仿生机械手样机、Arduino MEGA 2560开发板、电源、变压器、面包板等硬件基础，具体见下图。
![[项目基础.png]]
此外，单位已完成Leap Motion控制器的购买。
## 3. 项目技术路线
本项目的技术路线如下图：
![[技术路线.png]]
首先利用Leap Motion采集手势信息，随后将信息在PC端进行处理并通过蓝牙模块发送至Arduino开发板，最后由开发板根据输入的信息驱动电机控制仿生机械手做出响应手势。
## 4. Leap Motion开发与手势信息提取
### 4.1 Leap Motion介绍
Leap Motion装有两个红外摄像头和4个LED，能够在合适的光照下以200帧的速度捕捉手部图像，经过内部特殊的算法处理后可以得到整个手部各个手指各个关节节点的位置和速度信息，精度高达0.01 mm。
### 4.2 驱动程序
Leap Motion被电脑识别需要安装相应的驱动程序，而驱动程序有很多个大版本，最早的是V2，目前最新的是V5，官网仅提供V5版本的安装[Download Ultraleap's Hand Tracking Software](https://developer.leapmotion.com/tracking-software-download)。官方针对不同大版本有不同的示例应用[Ultraleap Gallery – Demos and experiments from the Ultraleap and our community](https://gallery.leapmotion.com/) ，其中利用手势对电脑进行控制的主要是基于V2、V4版本，V5版本主要是VR相关应用。
在查询Leap Motion控制机械手相关的工作时，发现大多都是十年之前的项目，其中介绍的最为详细的是[Robot Hand Control: Leap Motion, JavaScript, Node.js, and Arduino! : 5 Steps (with Pictures) - Instructables](https://www.instructables.com/Robot-Hand-Control-Leap-Motion-JavaScript-Nodejs-a/)。它使用的是V2版本的驱动程序并基于Node.js进行后续开发，主要是得益于Node.js的leapjs库。然而leapjs库最高支持到v12.0.0版本的Node.js，目前最新版本是v18.16.1；类似地，leapjs所依赖的库的版本也都相对较低。无论使用新老版本的Node.js，我均无法按照上面项目的流程成功搭建Leap Motion开发环境，推测原因是操作系统不适配。经在官方Discord社区发帖求后[Discord | "Using leap motion to control a robotic hand" | Ultraleap Developer Community](https://discord.com/channels/994213697490800670/1127775641496993794)，最终决定还是使用最新版V5的驱动程序以保证稳定性。
>Troubleshooting：安装最新版驱动之后我的电脑依然有一段时间识别不到Leap Motion，在依照官方的Troubleshooting[Troubleshooting Guide V5 (Gemini) – Ultraleap Support (leapmotion.com)](https://support.leapmotion.com/hc/en-us/articles/4406124780177-Troubleshooting-Guide-V5-Gemini-)进行相应操作未果后，又多次重装驱动重启电脑依然无法正常工作。直到第二天再次重装才终于正常，期间我没有任何动作，异常及恢复正常的原因均不明。**所以如果遇到相同的情况，或许等待一段时间就会有所改观。**
### 4.3 开发环境
V5版本驱动程序主要是基于C语言进行开发，完成驱动下载后，相应文件夹中会出现`./LeapSDK`这一子文件夹，其中就包含了搭建Leap Motion开发环境所需的各种文件以及一些示例程序。由于VSCode中修改配置文件相对不那么直观，我也没有找到操作方法，下面主要是使用Visual Studio 2022进行开发环境的搭建，参考博客[ Leap Motion开发（一）下载安装以及环境配置](https://blog.csdn.net/qq_39006214/article/details/122179118?ydreferer=aHR0cHM6Ly93d3cuZ29vZ2xlLmNvbS8%3D?ydreferer=aHR0cHM6Ly93d3cuZ29vZ2xlLmNvbS8%3D)总结出主要步骤如下：
- 找到Leap Motion驱动的安装目录`./Ultraleap`
- 项目-属性-连接器-输入-附加依赖项添加LeapC.lib的完整路径，对于我是`D:\Program Files\Ultraleap\LeapSDK\lib\x64\LeapC.lib`
- 项目-属性-添加包含目录，添加`LeapSDK`中`./include`文件夹的目录，对于我是`D:\Program Files\Ultraleap\LeapSDK\include`
- 将`LeapC.dll`所在的文件夹目录添加至环境变量（与`LeapC.lib`同一文件夹），对于我是`D:\Program Files\Ultraleap\LeapSDK\lib\x64`
- 项目源文件中添加`.Ultraleap/LeapSDK/samples`中的`ExampleConnection.c`，`ExampleConeection.h`。
在源文件中添加任意其他示例程序，点击`开始运行（不调试）`项目应可以正常运行。至此，开发环境搭建完成，电脑能够通过C语言程序访问并处理Leap Motion捕捉的手部信息。项目实际使用时只需要删除刚才添加的示例程序，在源文件中添加附件中的`LeapMotionControl.c`即可。
注意，VS2022默认的字符编码方式是unicode编码，即为占两个字节的宽字符，本项目中需要将字符编码方式调整为单字节的ASCII字符，项目→属性→高级 中可以调整字符的编码方式。
### 4.4 手势信息
Leap Motion能够通过Leap Motion开发接口包含丰富的数据结构、函数等，具体可参照官网帮助文档[Tracking API - Ultraleap documentation](https://docs.ultraleap.com/tracking-api/)，总体数据结构如下图所示（图片转自[【新手入门】全网最全Leap Motion技术总结](https://blog.csdn.net/qq_41858463/article/details/118023776)）。
![[数据结构.png]]
在完成开发环境搭建、将Leap Motion连接至电脑后，C语言程序可通过`GetFrame()`函数得到指向`LEAP FRAME`结构体的指针变量，进而得以访问其中包含的所有信息。
## 5. 远程通信
本项目想要实现对于机械手的无接触远程控制，而Leap Motion的识别范围有限，故作为信息输入端的电脑和作为输出端的Arduino板之间的远程通信就成为了不可缺少的一环，我们使用蓝牙模块来实现这一功能。
### 5.1 串口蓝牙模块
蓝牙串口是基于SPP协议(Serial Port Profile)，能在蓝牙设备之间创建串口进行数据传输的一种设备，广泛应用在具有蓝牙功能的电子设备上。考虑到模块连接与AT指令模块设置的便捷性，本项目采购了两个相同的HC-06蓝牙模块，分别连接于电脑和Arduino板以实现两者之间的通信。关于HC-06蓝牙模块的详细介绍见[[HC-06.pdf]]
#### 5.1.1 模块与单边机的连接
HC-06共VCC、GND、RXD、TXD四个引脚，VCC接3.3V供电，GND接GND，RXD与TXD交叉连接（即将单片机上与HC-06 TXD连接的引脚设置为RXD，反之亦然）。蓝牙模块通过单片机连接至电脑时，其AT指令设置可通过在单片机中烧写指定程序进行，具体可参考博客 [Arduino实现两个HC-05蓝牙模块控制蓝牙小车](https://blog.csdn.net/Smile_h_ahaha/article/details/101217839?ydreferer=aHR0cHM6Ly93d3cuZ29vZ2xlLmNvbS8%3D?ydreferer=aHR0cHM6Ly93d3cuZ29vZ2xlLmNvbS8%3D)，
#### 5.1.2 模块与电脑的连接
首先关于为什么需要采用第二个蓝牙模块而不使用电脑自己的蓝牙功能，原因是经各种尝试（操作流程见[Arduino教程：通过HC-06蓝牙让电脑控制Arduino](https://zhuanlan.zhihu.com/p/20081026)），Win11系统的电脑的蓝牙搜索不到蓝牙模块。而手机通过特定的串口蓝牙APP能够搜索到，这就排除了硬件问题，故目前认为是Win11操作系统不支持串口蓝牙连接，因此使用第二个蓝牙模块连接电脑实现两者之间的通信。
蓝牙模块连接电脑需要USB-TTL转接口，转接口与HC-06针脚的连接方式与单片机相同。
#### 5.1.3 蓝牙模块AT指令设置
安装串口调试助手（安装包见附件），单独将其中一个蓝牙模块与电脑连接，模块LED闪烁即为连接正常。
- 打开串口助手，波特率设置为HC-06默认的9600，数据位8，停止位1。
- 打开连接蓝牙模块的串口。
- 发送`AT`，若一切正常应能看到串口回复`OK`。
- 设置蓝牙配对密码`AT+PIN1111`，或者任意四位数。
- 更改模块主从模式`AT+ROLE=S`或`AT+ROLE=M`。
- 更改蓝牙波特率为38400：`AT+BAUD6`，权衡数据传输速度和稳定性
另一个蓝牙模块的设置流程类似，需要保证两者配对密码一致、波特率一致、一个主模式一个从模式，设置完成后两模块上电即可自动配对，LED灯常亮表示配对成功。
### 5.2 数据传输
为方便后续的电机控制需要利用蓝牙传输浮点型或整型数据，而常规蓝牙模块是按照单个字符信息传送和读取的，为此需要进行数据类型的转换。
#### 5.2.1 数据类型转换
最初的想法是将浮点数3.14转换为字符串"3.14"进行传输，但所需用到的函数`sprintf()`会使得程序卡住，原因不明；另外实际需要传输的浮点数通常有很多小数位，例如154.608429...，若是将其原样转换为字符串则会占用很多字节，不利于高效率传输。
考虑到单精度浮点数仅占4字节的空间，故采用union联合体将浮点数和4字节字符串封装在一起，浮点数用于赋值，字符串用于传输，这样不但减少了数据占用也保证了数据精度。
对于整形数据，需要注意的是Arduino环境中（至少是针对项目中使用的Arduino MEGA 2560来说）的整形变量长度仅为两个字节，对应的并非是C语言中的整形int而是短整型short。因此，在使用union封装的时候应使用2字节字符串。
#### 5.2.2 数据传输与接收方式
在数据发送端将浮点数拆分为四个字节之后，需要接收端将特定字节重新组合为正确的数据。
最初的想法是接收端每收到四个字符，就将其组合起来输出为浮点数。然而串口蓝牙传输本身容易受干扰，一旦出现传输错误如漏传一个字节，上述策略将导致后续的数据全部出错，传输稳定性极差。
故目前的解决方案是在需要传输的浮点数中间加入间隔符'S'，这样接收端在接收到'S'之后，就将后续的接收到的4个字节组合为浮点数，这样就算数据传输出现了错误，也能保证下一个数据不受影响。对于整型变量传输，只需要将后续接收4字节调整为2字节。
不过，这一策略也有弊端，即浮点数所包含的4字节数据中也可能存在被编译为'S'的字节，从而与间隔符发生混淆。如果发生这种情况，可能会导致附近的两三个数据传输均发生错误，影响范围还算可以接受，目前还没有想到更好的策略来解决这一问题。
#### 5.2.3 传输速率
采用9600波特率时，每秒钟能够传输的手势信息较少，这将会导致控制过程有较高的延迟。将波特率提高到38400后，信息发送的速度能够得到保证，然而信息接收速度不能与发送速度匹配。利用测试程序`temp.c`发送1000个浮点数，接收端只收到了400个左右，因此我们在信息发送端每次传输信息过后加了一点延迟，使得接收端正确的接收所有信息之后，再执行下一次信息发送。经实测，信息发送后加上10ms的延时就已经能够实现信息的正确传输。
## 6. 控制策略
我们目前所使用的仿生机械手仅有6个电机，分别用于控制各个手指的弯曲和大拇指的横向摆动，有限的自由度数实际上就限制了我们控制策略的复杂度，毕竟现有的机械手本身就无法实现复杂手势。
因此，我们采取较为简单的控制策略，即通过各个手指指尖与掌心的距离来控制对应驱动电机的转动角度。首先利用Leap Motion编写程序实时输出各个手指指尖到掌心的距离（见`LeapMotionControl.c`中最后注释掉的部分），记录距离的最大值（五指张开的情况）与最小值（握拳的情况），分别对应电机角度最小值0和最大值270，中间值采用线性映射。
## 7. 项目成果
![[总体平台.jpg]]
项目全平台搭建见上图，手势控制效果视频参见[Leap Motion控制器+串口蓝牙模块远程控制机械手_哔哩哔哩_bilibili](https://www.bilibili.com/video/BV1714y1q76S/?vd_source=1092069266b99de1a694e955a77b2550)。
## 8. 总结展望
本项目实现了基于Leap Motion和串口蓝牙模块的仿生机械手远程控制，主要是编写了Leap Motion开发程序以及串口蓝牙通信程序，采用简单的控制策略实现了目标功能。
由于暑期实践时间有限，工作较为粗糙，还存在许多可以优化的地方：
1. 一个就是前面提到的通信策略问题，即使加了一个间隔符，数据传输的稳定性依然不是太强，视频中也可以看到手指有明显抖动，猜测是数据传输错误导致的，后续可能需要考虑一下鲁棒性更强的通信策略。
2. 在Discord求助帖[Discord | "Using leap motion to control a robotic hand" | Ultraleap Developer Community](https://discord.com/channels/994213697490800670/1127775641496993794)中，回答里面提到的整体工作框架包含了Unity，而在实际项目进行过程中并没有发现需要它的环节。结合过程中搜集的资料[(7) Making an Animatronic Hand Controlled by Leap Motion - YouTube](https://www.youtube.com/watch?v=WQq8XFl_ZAI)，如果想要实现诸如动作复现之类的更多功能，可能需要借助Unity Plugin。如果后续有机会对项目进行完善的话，可以特别注意一下。
3. Leap Motion对于手势的识别并不十分完美，后三个手指单独进行弯曲时常常识别不到位。不过目前官方已经推出了Leap Motion 2，如果想要实现更精密的无接触手势控制，可以尝试一下最新的硬件。
4. 本项目暂未尝试将波特率进一步提高是否能够在保证稳定性的前提下更快的传输数据，项目的后续完善过程中可做相关尝试。修改蓝牙、程序波特率的流程如下：
	- 电脑连接的串口蓝牙：打开串口助手，用当前的波特率打开串口，输入AT指令修改蓝牙的波特率。
	- Arduino连接的蓝牙：将单片机连接电脑，首先使用当前波特率烧写AT指令调试程序`AT-Set.ino`，输入AT指令修改蓝牙波特率。
	- Arduino程序：将正式程序`test.ino`中初始化部分蓝牙与串口的波特率进行相应修改后，烧写至Arduino。
	- C语言程序：找到程序`LeapMotionControl.c`中使用的Openserial()函数，修改函数参数中的波特率。
## 9. 其他参考资料
综合参考：[(5) Robotic hand project : leapmotion (reddit.com)](https://www.reddit.com/r/leapmotion/comments/lkepmb/robotic_hand_project/)
蓝牙通信测试程序编写：[C语言 串口读写简单例子(windows)](https://blog.csdn.net/nanfeibuyi/article/details/125984648?ydreferer=aHR0cHM6Ly9jbi5iaW5nLmNvbS8%3D?ydreferer=aHR0cHM6Ly9jbi5iaW5nLmNvbS8%3D)