---
name: 每步写commit描述并确认
description: 完成每个子任务后准备git commit描述，且每步须经用户确认后才能继续
type: feedback
---

1. 完成每个子任务（如 1.2、1.3 等）后，须给出该步骤对应的 git commit 描述（不实际执行 commit，仅给出描述文本）。
2. 每完成一个子步骤，必须让用户确认后才能继续下一步。

**Why:** 用户希望每一步有清晰的版本记录，方便回溯和 review；同时确保每一步实现方向正确。

**How to apply:** 每完成一个子任务后，展示改动摘要 + 建议的 commit message，等待用户确认后再继续。
