# 2015-11-My_First_C_Project
2015년도 당시 C언어와 자료구조를 공부하고 직접 만든 회원 관리 프로그램입니다.
아는거라곤 정말 C언어 문법을 배우며 함께 알게된 자료구조 밖에 없던시절이라, 코드가 좀 더럽습니다. <br/>
하지만 거의 2~3주동안 여러가지 기능을 추가하고, 밤새면서 버그를 잡고 기능추가하며 개발의 재미를 맛봤던 프로젝트였습니다.<br/>
1학년 때 재미로 만든 토이 프로젝트였지만 1년 뒤 학교 기말 프로젝트에 약간만 수정 후 재사용(?)하여 한 학기 A+을 날로 먹었던 기억이 있네요. 

<hr/>
<br/>

# 원라인에디터

C언어에서 제공하는 콘솔입출력 (Scanf)를 사용하지 않고 수업시간에 배웠던 연결리스트와 getchar를 이용하여 나만의 콘솔 입력 메서드를 만들었습니다.<br/>
키보드 입력, 플레이스 홀더기능, 숫자만 입력가능, 최대길이 지정등 GUI의 텍스트 에디터처럼 여러가지 기능을 할 수 있게 구현했던 기억이 있습니다.

```c
STICK textwindow(const POS stpos, char *string, int *key, STICK cntl, USE menu, int *len);
//입력 문자열	  시작위치  문자열		  입력키	텍스트헤더	메뉴종류	최대길이(x 인설트여부)

STICK addnode(STICK cntl, char item);

//연결리스트 노드 추가용 함수, item(글자) 삽입 후 텍스트 연결리스트를 반환함
STICK delnode(STICK cntl, int ins);

//연결리스트 노드 삭제용 함수 ,삭제 후 텍스트 연결리스트를 반환함
int wherecur(STICK cntl, INTCUR menu);
//연결리스트 커서 추적 함수( 현재위치를 x좌표, 즉 정수값으로 반환)
```

<hr/>
<br/>

# 초기화면

그 당시에는 개발공부를 시작한지 1년도 안되었던 시점이라 데이터베이스가 무엇인지도 잘 몰랐습니다.<br/>
그래서 사용자 정보, 프로그램에서 생성되는 데이터를 `members.dat` 에 저장하였습니다.<br/>
만약 `member.dat` 이 없는 상태로 프로그램을 실행한다면 자동으로 생성합니다.

![image](https://user-images.githubusercontent.com/26001202/122553599-dbf44500-d072-11eb-86d8-e28d5c40a126.png)
![image](https://user-images.githubusercontent.com/26001202/122553394-933c8c00-d072-11eb-8465-f48d4a5983b8.png)
<br/>
비밀번호를 5번 이상 틀리게 된다면 프로그램이 종료됩니다. <br/>
프로그램이 종료 된 횟수는 데이터파일에 저장되며, 정상 사용자가 로그인 했을 경우 누군가가 비밀번호 입력을 시도하다가 프로그램이 종료되었음을 알려줍니다.

![image](https://user-images.githubusercontent.com/26001202/122553843-283f8500-d073-11eb-9197-b0a5b6562822.png)
![image](https://user-images.githubusercontent.com/26001202/122553861-2ecdfc80-d073-11eb-91d1-4fbaa625f469.png)

<hr/>
<br/>

# 초기화면

아래 그림과 같은 UI를 가지고 있으며 키보드로 동작합니다.<br/>
어떤 동작을 하던 ESC를 1번 누르면 종료하겠냐고 물어보고, 한번 더 누르면 메인 메뉴로 돌아 갈 수 있으며 원하는 입력이나 알림 메시지를 보여 줄 수 있습니다.

![image](https://user-images.githubusercontent.com/26001202/122554208-a865ea80-d073-11eb-895a-711cb7aeb605.png)

<hr/>
<br/>

# [F1] 회원등록

회원을 등록 할 수 있습니다. 학번이 중복되면 등록 할 수 없습니다. (안내메시지 나옴)
scanf가 아닌 직접 만든 원라인에디터를 사용하기 떄문에 플레이스홀더(특정 값만 입력가능, 000-000-000) 기능과 커서이동, Tab키로 다른 입력칸으로 이동 할 수 있습니다.<br/>
입력이 완료되면 저장할건지 물어보고, 취소하거나 현재 입력하고 있는 값을 수정 할 수 있습니다.<br/>
<br/>
나름 콘솔창으로 GUI 텍스트 에디터처럼 여러가지 기능을 하나하나 추가하는게 재밌었던 기억이 있네요.

![image](https://user-images.githubusercontent.com/26001202/122554833-6f7a4580-d074-11eb-9775-c55e0c9f1596.png)

<hr/>
<br/>

# [F2] 회원수정
식별번호로 등록되어있는 회원을 수정합니다.<br/>
만약 등록된 회원이 1명도 없다면 `등록된 회원이 없습니다` 라는 메시지만 뜹니다.<br/>
식별번호를 이용해 회원을 수정 할 수 있으며 식별번호가 기억나지 않는다면 수정메뉴에서 잠깐 **[F4 목록]** 으로 이동하여 등록된 회원을 확인 할 수 있습니다.<br/>
여기도 마찬가지로 텍스트 에디터의 여러가지 기능을 지원합니다. 입력 도중 취소하고 메뉴로 이동 가능

![image](https://user-images.githubusercontent.com/26001202/122555071-c4b65700-d074-11eb-9e39-ba1cef593d42.png)

<hr/>
<br/>

# [F3] 회원삭제
식별번호로 등록되어있는 회원을 삭제합니다.<br/>
수정에 있는 모든 편의기능이 제공되며, 여기에서는 목록을 띄워놓고 삭제할 회원을 볼 수 있습니다.

![image](https://user-images.githubusercontent.com/26001202/122555473-4dcd8e00-d075-11eb-9c64-bb6b05c98bb9.png)

<hr/>
<br/>

# [F4] 회원목록
등록된 회원을 확인하고, 검색하거나 정렬하여 볼 수있습니다.<br/>
수정이나 삭제 도중 회원목록 메뉴로 이동하여 확인 할 수 있습니다.

![image](https://user-images.githubusercontent.com/26001202/122556084-0dbadb00-d076-11eb-86b7-61688eb61b0b.png)


<hr/>
<br/>

# [F6] 관리자 (파일출력, 비밀번호변경)

회원명단을 텍스트파일로 출력 할 수 있습니다. 현재 디렉토리에 지정한 파일명으로 출력됩니다. 

![image](https://user-images.githubusercontent.com/26001202/122556368-60949280-d076-11eb-897f-7a08dc734029.png)

<br/>
비밀번호를 변경 할 수 있습니다. 단, 기존 비밀번호를 한번 더 입력해야 합니다.

![image](https://user-images.githubusercontent.com/26001202/122556578-a8b3b500-d076-11eb-8ada-a3984639b0ab.png)


<hr/>
<br/>

# [F7] 종료
프로그램을 종료합니다. 정말로 종료할 건지 한번 더 확인합니다.

![image](https://user-images.githubusercontent.com/26001202/122556647-bb2dee80-d076-11eb-9060-482ea1298150.png)

![image](https://user-images.githubusercontent.com/26001202/122556757-da2c8080-d076-11eb-8b46-4815a18c8b19.png)
