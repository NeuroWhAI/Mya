# Mya - Serval's Programming Language

![Title Logo](./logo.png)

## Hello, World!

- Brainf**k style
```
으먀먀아먀아
먕! 먀먀아먀먀-
먀먀~ 먀먀먀먀.
먀먀~ 먀먀아먀..
먀먀~ 먀먀먀.
먀먀~ 먀먀.
먀아먀아아~ 먀아먀.
먕? 먀아먀먀아먀-
먀먀~ 먀먀아. 먕,
먀먀~ 먀먀. 먕,
먀먀먀먀. 먕,,
먀먀먀. 먕,
먀먀~ 먀먀.. 먕,
먀아먀아~ 먀먀먀먀먀. 먕,
먀먀~ 먕,
먀먀먀. 먕,
먀아먀먀아. 먕,
먀아먀아아.. 먕,
먀먀~ 먀먀. 먕,
먀먀~ 먕,
```

## Spec

```
[인터프리터]
메모리 : int 배열. 포인터의 이동에 따라 무한히 늘어남.
스택 : push, pop이 가능한 int 스택.
레지스터 : 읽고 쓸 수 있는 int 값 하나.
헤드 : 실행할 명령어 번호.
포인터 : 메모리의 특정 위치 번호.

[명령어]
앞 수식어, 함수, 뒷 수식어로 이루어져 하나의 명령어가 됨.

[앞 수식어] : 0개 이상 1개 이하.
없음 : 아무것도 안함.
우 : 함수가 반환한 값을 스택에 넣음.
으 : 함수가 반환한 값을 메모리에 씀.
움 : 함수가 반환한 값을 레지스터에 씀.

[함수] : 1개 필수.
먀[먀아]* : 이진수 표기법(먀(1), 아(0))으로서 해당하는 값 반환. 첫 먀는 무시. 첫 먀 다음에 아(0)로 시작하면 음수.
먕 : 메모리 값을 반환.
먓 : 스택의 값을 꺼내 반환. 없으면 0을 반환.
먐 : 레지스터의 값을 반환.
먘 : 표준입력에서 1바이트를 읽어 반환.

[뒷 수식어] : 0개 이상.
없음 : 아무것도 안함.
! : 반환된 값이 레지스터의 값과 다르면 헤더 1 증가. (헤더의 자동증가와 겹쳐 결론적으로 점프를 유발)
? : 반환된 값이 레지스터의 값과 같으면 헤더 1 증가. (헤더의 자동증가와 겹쳐 결론적으로 점프를 유발)
~ : 반환된 값만큼 포인터 증가.
- : 반환된 값만큼 헤더 증가. (이때 헤더는 자동으로 증가하지 않음)
. : 반환된 값을 메모리에 더함.
, : 반환된 값을 표준출력으로 보냄.
; : 반환된 값에 해당하는 위치로 헤더 이동. (이때 헤더는 자동으로 증가하지 않음)

[명령어 실행 순서]
함수 -> 앞 수식어 -> 뒷 수식어
```