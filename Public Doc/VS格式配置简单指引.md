# <center>VS格式配置简单指引</center>





<p align = "right">陈天宇</p>

<p align = "right"> 2020-11-18</p>

## 一. 前言

&emsp;&emsp;众所周知tab和space一直存在争论，而我们的代码里统一采用space代替tab，下文介绍如何为VS配置自动修改。

## 二. Format On Save

&emsp;&emsp;首先需要安装插件Format On Save，该插件可以起到自动替换的效果。

## 三. VS配置

- 在`Options->Text Editor->General`目录下，开启`View whitespace`和`Tab/Space`方便查看。

- 选择`Options->Text Editor->C/C++`
  - `Tabs`中，选择`Insert spaces`，调整为4
  - `Formatting->General`中，`When I paste`选择`Do nothing`
  - `Formatting->Spacing`中，调整`Pointer/reference alignment`，选择`leave unchanged`

- 选择`Options->Format On Save`，`EnableForceUtf8WithBom`至`EnableUnifyLineBreak`几个选项，除了`EnableFormatDocument`设置为False，其余设置为`True`，`LineBreak`设置为`Unix`。









```c
static int kthread(void *_create)
{
    /* Copy data: it's on kthread's stack */
    struct kthread_create_info *create = _create;
    int (*threadfn)(void *data) = create->threadfn;
    void *data = create->data;
    struct completion *done;
    struct kthread *self;
    int ret;

    self = kzalloc(sizeof(*self), GFP_KERNEL);
    set_kthread_struct(self);

    /* If user was SIGKILLed, I release the structure. */
    done = xchg(&create->done, NULL);
    if (!done) {
        kfree(create);
        do_exit(-EINTR);
    }

    if (!self) {
        create->result = ERR_PTR(-ENOMEM);
        complete(done);
        do_exit(-ENOMEM);
    }

    self->data = data;
    init_completion(&self->exited);
    init_completion(&self->parked);
    current->vfork_done = &self->exited;

    /* OK, tell user we're spawned, wait for stop or wakeup */
    __set_current_state(TASK_UNINTERRUPTIBLE);
    create->result = current;
    /*
     * Thread is going to call schedule(), do not preempt it,
     * or the creator may spend more time in wait_task_inactive().
     */
    preempt_disable();
    complete(done);
    schedule_preempt_disabled();
    preempt_enable();

    ret = -EINTR;
    if (!test_bit(KTHREAD_SHOULD_STOP, &self->flags)) {
        cgroup_kthread_ready();
        __kthread_parkme(self);
        ret = threadfn(data);
    }
    do_exit(ret);
}
```

