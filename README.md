# 队式16 - Deep Blue

欢迎来到队式16的开发仓库~

本仓库目前包括了逻辑组和平台组，未来…会不会有界面组和3D组呢(⊙_⊙?)

目前入驻的暂时只有：

**逻辑组**：郭一隆 (@Nuullll), 孙胜扬 (@ssydasheng), 杨幻睿 (@yanghr), 王殷豪 (YacratesWyh)

**平台组**：李思涵 (@ThomasLee969), 刘家硕 (@Liudada), 阮磊 (@tristoner-RL)

谁知道科协git肿么了……


## 公告公告，大家看这里看这里~

还没有学会git的，现在赶快去现学现卖~

[Git教程](http://www.liaoxuefeng.com/wiki/0013739516305929606dd18361248578c67b8067c8c017b000) by 廖雪峰

[Git简明指南](http://rogerdudler.github.io/git-guide/index.zh.html) by rogerdudler（据说是雨泽学长推荐）

然后去学习一下一种[正确的分支姿势](http://www.ruanyifeng.com/blog/2012/07/git.html)，这次的开发的分支策略就是基于这个的。 ***真的很重要！！！***


## 当前任务
图例：

***紧急的任务***

一般的任务

*关系不大的任务*

### 逻辑组
* ***确定 逻辑/平台 接口***
* 编出能跑的游戏
* *更新文档，使得其与basic.h保持一致*

### 平台组
* ***确定 逻辑/平台 接口***
* 写出对战流程
* 完成和选手程序方面的通信
* 完成能黑箱运行的对战程序

## 已完成
√ 提供 Markdown/LaTex 格式的选手文档 (@ThomasLee)

## 关于Git
请大家 ***不要*** 使用中文文件名或目录名 = =

请大家 ***不要*** 直接对 **master** 分支进行修改，而是将 **develop** 分支作为主要开发分支。（为了保险起见我还是把 **master** 设为protected了…Orz）

请大家 ***不要commit写到一半的代码***，对每次commit ***附上清晰简明的概要***，并且 ***在本地测试没有问题之后再push***

请大家在merge时 ***使用*** `--no-ff` ***参数***，以保持branch层次的清晰

鉴于惨痛的经验…为了保持分支图的清晰，如果在push时发现远程仓库下的对应分支已被修改，并且预期不会有太多的conflict， ***请不要直接使用*** `git pull`， ***而是使用*** `git pull --rebase`，以避免不必要的merge


## 看完了教程还是好糊涂…有没有实际工作流程的例子？

下面是个人摸索出的，比较科学的工作流程……

### 1. 准备阶段

```bash
$ git clone http://git.eesast.com/ThomasLee/teamstyle16.git  # 克隆仓库到当前目录下
$ cd teamstyle16  # 进入本地仓库文件夹
$ git status  # 确认下处在 develop 分支
```

如果还想要仓库内的其他分支（如 master)
```bash
$ git checkout -b master origin/master  # 创建本地分支，并跟踪远程分支
$ git pull  # 更新本地分支到最新状态
```


### 2. 开始某一工作目标

```bash
$ git checkout develop  # 来到 develop 分支
$ git pull  # 获取最新的工作进度
$ git checkout -b some_work  # 分支出 some_work 分支，并转到该分支
```

如果是需要多人合作的分支，则可能需要 push 到远程库
```bash
$ git push -u origin some_work  # 推至远程，同时设置为跟踪该远程分支
```


### 3. 工作目标进行时

加入了新文件 `some_new_file`
```bash
$ git add some_new_file
```

修改了一系列已经添加的文件，一同提交
```bash
$ git commit -a
```

develop 分支上出现了比较重要的更新，需要立即合并到当前分支
```bash
$ git checkout develop  # 来到 develop 分支
$ git pull  # 获取最新的工作进度
$ git checkout some_work  # 来到工作分支下
$ git merge --no-ff develop  # 将最新更新与自己已有工作进度合并
```


### 4. 完成工作目标
```bash
$ git checkout develop  # 来到 develop 分支
$ git pull  # 确保 develop 分支与远程同步
$ git merge --no-ff some_work  # 将工作成果合并进 develop 分支
$ git push  # 将合并后的 develop 分支推至远程仓库
```

如果很不幸，有人在 merge 的时间里更新了 develop 分支导致 push 失败， ***请在 pull 时带上*** `--rebase`，以保持分支图的清晰
```bash
$ git pull --rebase  # 将本次修改放在此修改之后
$ git push  # 推回远程仓库
```

最后别忘了删除 some_work 分支
```bash
$ git branch -d some_work  # 删除 some_work 分支
```

如果之前将该分支推至远程库，则还要从远程库删除
```bash
$ git push origin :some_work  # 将一空分支 push 到远程库的该分支，即删除
```


大家注意，所有操作都保持了 develop 分支 **在本地与远程的同步**

这里的思路是 **本地的 develop 分支是远程 develop 分支的直接体现**

所有本地 develop 分支上的改变，都应对应着远程 develop 分支上的改变

故在 develop 分支可能发生冲突时，请使用 `git pull --rebase` （如果不确定，一直用它就好了…）

其他类似的分支同理……


## 命令行用起来太捉急啦，有木有图形化的工具？

推荐[SourcetTree](http://www.sourcetreeapp.com)

![SourcetTree界面](http://www.sourcetreeapp.com/images/sourcetree-hero-mac-log.png)

不过建议大家主要用这类工具看看分支树和修改历史，或者便捷地 commit……当然，如果你 ***清楚地*** 知道自己在做什么的话，用它而抛弃命令行也不是什么大事 = =

---------------------------------------------------------------------

大家加油~🙏
