### 简介

软件名称：铁路12307

适用平台：Android （API 23 - 28）

开发工具：Android Studio

开发环境：windows 10, ubuntu 16.04

测试平台：MI 8，Nexus 5X（API 26）

开发技术：Java+xml

开发人员：庄永昊（后端），陆嘉馨（通信等），毛昕渝（后端），吴润哲（前端）

### 模块划分

#### 通信模块

1. Socket 通信
   
   采用Socket通信。为实现多个用户同时向后端发送消息的情况，后端循环监听前端的请求，前端创建Socket后向后端发送请求，后端接收后新建一个线程处理相应请求，并把反馈信息发送给前端。

2. 加密
   
   为了保证通信信息的安全，对通信的信息进行加密。

   为了保证前后端JAVA与C++的统一，加密先采用AES加密，选取ECB加密模式，PKCS5Padding填充，设置加密字符集为UTF-8。AES加密后，再使用Base64进行加密。

   解密则先进行Base64解密，后使用对应的模式进行AES的解密。
   

#### 用户模块

1. LoginActivity：登录。
2. RegisterActivity：注册。

#### 购票退票模块

1. MainActivity：主界面，其中包括Fragment1（用于订购车票），Fragment2（用于查询已购车票），Fragment3（用于查询和修改个人信息）。
2. InformationActivity：在Fragment1即订购车次时选定车次相关信息后调用，用于返回查询到的车次信息。
3. BuyActivity：在InformationActivity中选定要购买的车票之后弹出的购买车票活动。
4. ReturnActivity：在Fragment2即查询已购车票后，进行退票时的活动。

#### 车次模块

只有管理员才能访问车次模块的内容。

1. AdminActivity：管理界面的主界面。右下角的按钮可以增加车次。
2. NewActivity：增加车次的第一个界面，用于设置车次的属性（ID，名称，车类，席别等）。
3. New2Activity：增加车次的第二个界面，用于增加站点和保存。
4. ModifyUserInfoActivity：管理员修改用户信息用。

#### 其他模块

1. CityPickerActivity：用于选择城市信息的活动，已废弃。
2. Client：通信用。
3. ProgressbarFragment：进度条类。
4. SelectStation：用于选择车站的活动。
5. SplashActivity：启动界面，未启用。
6. ClearEditText，SortAdapter，SortModel，TitleItemDecoration，WaveSiderBar：辅助SelecStation用，是它的一部分。
7. PinyinComparator，PinyinUtils：拼音相关，辅助SelecStation用。
8. Tools：工具类。

### 功能优化

1. Toast

   由于自带的Toasty颜色单一，我们使用了开源的Toasty组件，使提示信息的颜色更为丰富。

2. 车站选择

   我们内置了全中国的所有车站，并提供了一个友好的界面以方便用户进行选择。在该界面内，用户可以使用汉字，首字母等直接在上方的输入框输入以进行检索。用户也可以使用拖动右侧的26个英文字母进行检索。

3. 时刻/日期选择

   为了方便用户输入，我们使用了DatePicker和TimePicker来用户辅助用户输入时刻和日期。

4. 主界面导航栏

   为了提供更好的用户体验，我们把界面切换的按键设计为底部的导航栏。用户直接点击下部的三个按钮就可以轻松进行界面切换。

5. 登录

   由于ID难以记忆，为了更好的用户体验，我们支持用邮箱或手机代替ID来作为用户名以进行登录。

6. 图标与颜色

   为了用户的眼睛，配色以蓝白为主调，大部分图标取自Google的图标包。

