### 登录

由LoginActivity.java和activity_login.xml组成。

此界面要求用户输入用户名和密码进行登录。

前端检查合法性。若用户名或密码为空则提示错误。将数据传给后端，由后端返回登录结果。若登录成功则进入主界面，否则提示错误。

**目前传递给后端部分缺失，只要用户名和密码都不为空就认为登陆成功。**

### 注册

由RegisterActivity.java和activity_register.xml组成。

此界面用于用户注册。

前端检查合法性。若存在一项为空，或两次密码不一致，或手机号码不全是数字，或手机号码不是11位，或邮箱格式不正确则报错。将数据传给后端，由后端返回注册结果。若注册成功则返回登录界面以并要求重新登录，否则提示未知错误。

**目前传递给后端部分缺失，只要满足前端检查就认为注册成功。**

### 主界面

主界面分成三个Fragment：首页、订单、我的，Fragment之间的切换依赖于底部的导航栏。右上角有小型菜单栏，里面有：管理模式、关于。

MainActivity.java提供主要逻辑，activity_main.xml是空白布局，里面放Fragment。

#### 首页

由Fragment1.java和fragment_1.xml组成。

此界面用于查询车票信息。

在两个输入框内分别输入起点和终点（点击自动弹出CityPicker以进行选择）。中间的双箭头可以交换起点和终点。点击日期右边的修改图标可以进入日期修改。

点击查询车票后。前端检测两个站点的输入框是否非空。随后将所有信息传给后端，从后端接收到的车票信息使用putExtra方式以string[]参数传给InformationActivity，并加以显示。保证string[]参数中的每一个string的第一段字符串是车次id以便后续操作。

##### CityPicker

一个比较好看的城市选择界面。

采用StartActivityForResult方式切换至CityPickerActivity.class进行城市选择。 requestCode是1。

直接改TextView。

##### 查询结果

由InformationActivity.java和activity_information.xml组成。

可以按照一定格式显示符合条件的所有车票。保证string第一段是车次的id以便操作。点击任一车票后，获取点击项的id，传给后端，并由后端返回这种车票对应的票信息，通过putExtra方式全部传给BuyActivity。随后进入购买界面。

##### 购买车票

由BuyActivity.java和activity_buy.xml组成。

选定购买的车票类型后点提交订单。前端即将选定票类型curType传给后端。后端返回是否购票成功。

目前一次只能购买一张车票，购买后页面即消失。（后续可能加上一次买多张票）

#### 订单

由Fragment2.java和fragment_2.xml组成。

用于退票。

只有一个ListView，按照一定规则以String显示所有车票，保证第一段字符串是id。

单击后询问是否退票（尚未考虑一票多张的问题）。点击确定后向后端发送退票请求。后端返回退票结果。前端显示退票结果并刷新整个界面。

#### 我的

由Fragment3.java和fragment_3.xml组成。

用于修改用户信息。

**修改其他用户的权限功能还未加入，预计加入在管理模式中。**

**头像功能正在研发，可能会因发现没卵用而夭折。**

前端判断合法性，若除了新密码以外有一项为空，或手机号码不是11位，或邮箱格式有误则报错。将数据传给后端，由后端返回修改结果并显示。

### 关于

随便填字，预计放一些我们组的信息。

### 管理模式

由AdminActivity.java和activity_admin.xml组成。

只有权限为2，即管理员，才能进入。

右上角有删库菜单。

ListView中的车票信息按照惯例的规则显示，第一段字符串为id。

点击悬浮按钮可以创建车次。

手动输入ID或点击已有的车次，再点查找可发售、删除、修改车次信息。

#### 新建车次

##### 第一界面——新建车次

由NewActivity.java、New2Activity、activity_new.xml和activity_new2.xml组成。

第一个界面确定车次的属性（后续可能增加车类的选项栏）。将id传给后端以判断是否有重复id，有就报错。

选定票类型后，在下一界面新建车次时只有选中的票类型才能填写，其他均是N/A。

用putExtra方式把所有数据传到第二界面。

**前端还未判断各项非空。**

##### 第二界面——新建站点

点击提交则提交。

点击新建站点则进入新建站点对话框。

前端判断票价是否非空、是否是非负整数。

若通过检测则在指令字符串后面增加一串。当提交时将整个指令字符串传给后端。后端返回创建结果。

#### 发售、删除、修改车次信息

由AdminActivity.java中的部分和activity_modify_ticket.xml组成。

发售、删除、修改按下后，向后端发送请求。如果车票未被发售则可以进行以上操作并在操作后刷新。如果已经发售则报错。

修改实际上是先执行删除操作，然后用putExtra辅助切换至NewActivity进行新建车次操作。

#### 删库菜单

直接向后端发送删库，不需要返回值。随后刷新界面。

### Tools

手写的工具库。

### 造好的轮子

`Toasty` https://github.com/GrenderG/Toasty
`CityPicker` https://github.com/zaaach/CityPicker
