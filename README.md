一个简单的http服务器, 使用json方式可以与其通信, 调用poco库.

# 编译-运行-测试

环境配置(ubuntu): apt update && apt install libpoco-dev cmake g++

编译: cmake . && make

运行: ./demo students.json

测试: 运行demo的同时, 在另一个终端执行./test.sh, 可用于模拟与demo的交互

# API

## 列出待录取学生

- pos指名次序号
- pos_max = -1 pos_min = 0 时显示所有学生

POST:

/search

```JSON
{
    "pos_min": xxx,
    "pos_max": xxx,
}
```

RESP:

```JSON
{
    "students": [
        {"pos": xx, "name":"xxx", "age": xx}, 
        {"pos": xx, "name":"xxx", "age": xx},
    ],
    "status": "OK"
}
```

## 录取学生

POST:

/admit

```JSON
{
    "pos": [
        xxx,
        xxx,
    ]
}
```

RESP:

```JSON
{
    "status": "OK"
}
```

## 列出已录取学生

POST:

/list

```JSON
{
}
```

RESP:

```JSON
{
    "students": [
        {"pos": xx, "name":"xxx", "age": xx}, 
        {"pos": xx, "name":"xxx", "age": xx},
    ],
    "status": "OK"
}
```

## 上课

POST:

/course

```JSON
{
    "pos": [
        "xxx",
        "xxx",
    ],
    "course": "math"
}
```

RESP:

```JSON
{
    "status": "OK"
}
```
