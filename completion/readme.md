> **Note:**
> - 实际上 comlepe_all() 会将变量done设置为无穷大, 那么后面的wait_for_completon()将直接返回.
> - 使用多线程的过程中, 建议对单独线程创建独立的comlepe变量.
> - 实际使用注意线程退出时的情况，这里暂时是推荐使用 wait_for_completion_timeout 超时后再判断退出条件。
