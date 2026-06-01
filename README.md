# Tetris Project

C언어를 활용하여 터미널 환경에서 동작하는 Tetris 게임을 구현한 프로젝트입니다.
2차원 배열을 이용해 게임 보드를 관리하고, 블록 이동, 회전, 충돌 판정, 라인 삭제, 점수 계산 기능을 다루었습니다.
또한 랭킹 시스템과 Tree 자료구조 기반 위치 추천 기능을 구현하며, 자료구조가 실제 게임 로직과 의사결정 과정에 어떻게 활용될 수 있는지 학습했습니다.

## Project Structure

* `tetris.c`: Tetris 게임의 주요 기능 구현 코드
* `tetris.h`: 게임에 사용되는 구조체, 상수, 함수 선언
* `makefile`: 컴파일 자동화를 위한 Makefile

## Features

* 터미널 기반 Tetris 게임 실행
* 블록 이동 및 회전 처리
* 블록 이동 가능 여부 검사
* 라인 삭제 및 점수 계산
* 랭킹 정보 저장 및 관리
* Tree 자료구조 기반 블록 위치 추천

## Development Environment

* Language: C
* Environment: Linux/Unix terminal
* Compiler: GCC
* Tool: PuTTY
* Build Tool: Makefile

## What I Learned

이 프로젝트를 통해 2차원 배열을 활용하여 게임 보드와 블록 상태를 관리하는 방법을 익혔습니다.
블록 이동과 회전 과정에서 이동 가능한 위치를 검사하며, 게임 규칙을 조건문과 배열 인덱스 처리로 구현하는 과정을 경험했습니다.

또한 포인터와 구조체를 활용하여 게임 상태와 랭킹 데이터를 관리하는 방법을 학습했습니다.
Tree 자료구조를 활용한 위치 추천 기능을 다루면서, 자료구조가 단순한 이론이 아니라 가능한 경우의 수를 탐색하고 선택하는 의사결정 로직에 활용될 수 있다는 점을 배웠습니다.

## Build & Run

```bash
make
./a.out
```

## Future Improvements

* 추천 알고리즘 평가 기준 개선
* 코드 모듈화 및 함수 역할 분리 개선
* 게임 화면 출력 방식 개선
* 예외 상황 처리 보완
