# `Mini_lisp` 拓展功能

本 `Mini_lisp` 解释器主要按照 `Mini_lisp` 标准，同时对部分功能有修改，并添加了一些新的功能。关于标准文档，详见[标准文档](https://pku-software.github.io/mini-lisp-spec/)。

## 与 `Mini_lisp` 标准不同的地方：

### `append` 内置过程

本 `Mini_Lisp` 解释器实现的 `append` 内置过程允许不接受参数，这将返回空表。

```lisp
(append) ; 返回 ()
```

允许接受若干个列表，将其中的元素按顺序拼接成一个新的列表。

```lisp
(append '(1 2 3) '(a b c) '(foo bar baz)) ; 返回 '(1 2 3 a b c foo bar baz)
```

允许最后一个元素不是列表，将返回对子。

```lisp
(append '(1 2 3) 4) ; 将返回 '(1 2 3 . 4)
```

