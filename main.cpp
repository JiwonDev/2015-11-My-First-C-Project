#include <stdio.h>
#include <stdlib.h>
#include <conio.h>//getch() ,putch(), kbhit()
#include <windows.h> //커서 관련함수 ,system() , Sleep(딜레이함수)
#include <string.h> // strcmp(텍스트 비교), strcpy(텍스트 복사)

#define FILECHECK 2015642028//파일 고유번호, 내 파일이 맞는지 확인할때 사용
#define N 100//전체갯수
#define DATAFILE "members.dat"//데이터파일명
#define PRINTHEAD "Made in Jiwon"//출력파일(명단) 머리글
//////////////////////////////////////
//회원파일 구조
typedef struct members {
    int count;
    char id[12];//ID
    char name[13];//이름
    char phone[14];//전화번호
    char grade;//학년
} PERSON;//회원정보

typedef struct headers {
    //전체갯수와 삭제갯수를 나눈이유는 원래는 휴지통기능을 만들려다가
    //시간부족으로 무산되었음.. (휴지통 안에 있는갯수를 알기위해서)
    //(파일을 바로삭제하지않고 검색만 안되게해놨다가 나중에 관리자로 삭제)

    int filecheck;//고유번호, 파일생성시 사용
    int size;//등록한 전체갯수 (파일고유번호)
    int delsize;//삭제갯수 (실제 갯수를 알기위해서)
    int error;//틀린횟수
    char pw[12];//비밀번호
} HEAD;//회원헤드


///////////////////////////
// oneline editor 구현용 글자 연결리스트
typedef struct nodes {
    char data;//글자
    struct nodes *next;//다음
    struct nodes *prev;//이전
} NODE;//텍스트정보

typedef struct sticks {
    NODE *head;//연결리스트 헤드
    NODE *cur;//현재 커서
} STICK;//텍스트 헤드

///////////////////////////////
//좌표용 구조체와 쓰기편하게 각종 열거형
typedef struct pos {
    int x;
    int y;
} POS;

typedef enum {
    NONE = 1, ID, FILENAME, PHONE, GRADE
} USE; //textwindow 용
typedef enum {
    CUR = 1, MAX
} INTCUR;//wherecur 용
typedef enum {
    NO = 1, SOLID, NORMAL
} CURSOR_TYPE;//cursor_type 용
///////////////////////////////////////////////////

//#1 커서 관련 함수들
void cursor_type(CURSOR_TYPE c);

//커서모양을 바꿔줌
void gotoxy(int x, int y);

//해당좌표로 커서를 이동시켜줌
POS wherexy();
//현재 커서위치를 반환해줌

//#2 메뉴관련 함수들
int add_members(PERSON members[], int count, int remake);

//회원입력함수, remake는 수정용
int updata_members(PERSON members[], int count);

//수정(선택창)
int delete_members(PERSON members[], int count);

//삭제(선택창)
int sticktostr(STICK cntl, char *string);
//텍스트 연결리스트(나만의 scanf에서 반환한)를 문자열로

int print_members(PERSON members[], int count);

//목록
int search_members(PERSON members[], int count);

//검색
int array_members(PERSON members[], int count);
//정렬

int supervisor(PERSON member[], int count);

//관리자
int printfile(PERSON members[], int count);
//회원목록(파일)출력

//#3 나만의 scanf함수
STICK textwindow(const POS stpos, char *string, int *key, STICK cntl, USE menu, int *len);
//입력 문자열	  시작위치  문자열		  입력키	텍스트헤더	메뉴종류	최대길이(x 인설트여부)

STICK addnode(STICK cntl, char item);

//연결리스트 노드 추가용 함수, item(글자) 삽입 후 텍스트 연결리스트를 반환함
STICK delnode(STICK cntl, int ins);

//연결리스트 노드 삭제용 함수 ,삭제 후 텍스트 연결리스트를 반환함
int wherecur(STICK cntl, INTCUR menu);
//연결리스트 커서 추적 함수( 현재위치를 x좌표, 즉 정수값으로 반환)

////////////////////////////////////////
//전역변수 - 회원헤더(파일헤더, 데이터 파일 제일앞에 기록됨)
HEAD filecntl;


int main(int argc, char *argv[]) {
    POS stpos = {0, 0};//좌표저장용
    char string[30] = {0,};//문자열 저장용
    int count = 0, key = 0, len = 0, passlen = 10;//데이터 갯수, 입력키, 길이, 길이2(패스워드용)
    int temp = 0, i = 0, yes = 0;//변수 1, 변수 2
    STICK cntl = {NULL, NULL};//텍스트(scanf)용 연결리스트
    STICK none = {NULL, NULL};//아무것도 입력안할때 NULL 대용으로 만듬.

    PERSON members[N];// 회원정보가 저장되는 구조체 배열


    //### 1. 파일 읽기 	///////////////////////////////////////////////////////////
    FILE *fp;
    fp = fopen(DATAFILE, "rb");//읽기용 이진파일(read binary)로 파일포인터 생성


    //## 1-1 오류(파일이 존재하지않을시)
    if (fp == NULL) {
        printf("┌─────────────────┐\n");
        printf("│데이터 파일이 존재하지 않습니다.  │\n");
        printf("│                                  │\n");
        printf("│-[%12s]파일을 생성합니다. │\n", DATAFILE);
        printf("└─────────────────┘\n\n");

        //파일헤더 초기값 입력
        filecntl.filecheck = FILECHECK;
        filecntl.delsize = 0;
        filecntl.size = 1;
        filecntl.error = 0;
        strcpy(filecntl.pw, "0000");
        system("PAUSE");
        fclose(fp);

        //데이터파일 갱신
        fp = fopen(DATAFILE, "wb");

        fwrite(&filecntl, sizeof(HEAD), 1, fp);

        for (i = 0; i < count; i++)
            fwrite(&members[i], sizeof(PERSON), 1, fp);

        fclose(fp);
    }

        //## 1-2 아닐경우 (파일이 존재할경우)
    else {
        fread(&filecntl, sizeof(HEAD), 1, fp);
        //파일앞부분 (데이터헤더) 만 읽어온다.

        //# 1-2-1 만약 데이터 헤더의 값이 이상할경우
        if (filecntl.filecheck != FILECHECK || filecntl.error > 10000) {
            printf("┌───────────────────┐\n");
            printf("│[ERROR] 데이터 파일이 손상되었습니다. │\n");
            printf("│                                      │\n");
            printf("│     데이터파일명[ %-12s]      │\n", DATAFILE);
            printf("└───────────────────┘\n\n");
            fclose(fp);
            system("PAUSE");
            exit(-1);
        }

            //# 1-2-1 그외 정상일경우
        else {
            //feof = 파일이 끝(End of file)일 경우 1을, 아닐경우 0을반환
            while (!feof(fp)) {
                //파일을 members[count]에 읽고 count++
                fread(&members[count], sizeof(PERSON), 1, fp);
                count++;
            }
            //맨 마지막 count는 빈공간이니 --
            count--;
        }


    }
    system("cls");
    i = 0;
    //화면을 지우고 변수를 초기화합니다.


    //### 2. 암호 입력	///////////////////////////////////////////////////////////
    while (1) {
        Sleep(30);//반복문이 너무 빠르게 실행되는걸 방지하기위해서
        gotoxy(20, 6);
        printf("┌───────────────┐");
        gotoxy(20, 7);
        printf("│회원관리 프로그램 [verson 1.0]│");
        gotoxy(20, 8);
        printf("└───────────────┘");
        gotoxy(25, 10);
        printf(" 비밀번호 [ ");
        stpos = wherexy();
        printf("           ]");

        //textwindow에 이놈을 이렇게 주면 *로 출력합니다.
        string[0] = '*';
        if (filecntl.error == 0) {
            gotoxy(stpos.x - 8, stpos.y + 3);
            printf(">> [Tip] 초기비밀번호는 [ 0000 ]입니다.");
        }

        //나만의 scanf
        do { cntl = textwindow(stpos, string, &key, none, NONE, &passlen); } while (key != 13);
        sticktostr(cntl, string);

        if (strcmp(string, filecntl.pw) == 0) {//## 2-1만약 암호가 맞다면
            system("cls");
            if (filecntl.error > 3) {//암호 실패횟수가 3번을 넘는다면
                printf("[Waring] %d번의 비밀번호 입력 시도가 있었습니다.", filecntl.error);
                printf("\n\n    - 횟수를 초기화합니다.\n───────────────────\n\n");
                system("PAUSE");
            }
            filecntl.error = 0;//암호 실패횟수를 초기화합니다.
            break;//main으로
        } else {//## 2-2 암호가 틀렸다면
            gotoxy(stpos.x - 8, stpos.y + 3);
            filecntl.error++;
            i++;//암호 실패횟수와 변수를 +1합니다.(3번이상 틀리면 프로그램종료)
            if (filecntl.error > 3 && i > 3) printf(">> [Error] 비밀번호 오류로 종료됩니다.       \n\n");
            else printf(">> [Error] 잘못된 비밀번호입니다.                       \n\n", filecntl.error);

            if (filecntl.error > 3 && i > 3) {//## 2-2-1 3번이상 틀렸다면

                //데이터파일 갱신
                fp = fopen(DATAFILE, "wb");

                fwrite(&filecntl, sizeof(HEAD), 1, fp);

                for (i = 0; i < count; i++)
                    fwrite(&members[i], sizeof(PERSON), 1, fp);
                fclose(fp);
                Sleep(1000);
                exit(-1);
            }
            //나만의 scanf에 저장된 글자를 지워버립니다.
            cntl.head = NULL;
            cntl.cur = NULL;
        }

    }
    //나만의 scanf에 저장된 글자를 지워버립니다.
    cntl.head = NULL;
    cntl.cur = NULL;

    //### 3. 메뉴 	///////////////////////////////////////////////////////////
    while (key != 7) {
        //Sleep 도중 입력한 키값은 전부 버림.
        //kbhit()은 키보드 입력값이 있으면 1, 없으면 0을 반환
        while (kbhit()) getch();
        //getch() , 버퍼를 쓰지않고 키보드에서 바로 받아오는 함수를 쓰기에 fflush()가 먹히지 않습니다.


        system("cls");//화면 지우기
        gotoxy(0, 0);
        printf("┌[ 회원관리 ]───────────────────────────────┐\n");
        printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
        printf("│                                                                          │\n");
        printf("│ [F1]등록   [F2] 수정   [F3] 삭제   [F4] 목록      [F6] 관리   [F7] 종료  │\n");
        printf("└─────────────────────────────────────┘\n");
        stpos = wherexy();

        //나만의 scanf
        textwindow(stpos, NULL, &key, cntl, NONE, &len);
        gotoxy(stpos.x, stpos.y);

        switch (key) {//F1~F7 은 숫자 1~7로 반환합니다.
            case 1://등록부분
            {
                if (count < N)//최대갯수가 넘지않았다면
                {
                    yes = add_members(members, count, 0);
                    //등록을 실행합니다.
                    if (yes == 1)//등록되었다면
                    {
                        count++;
                        printf(" %-50s", "회원정보가 정상적으로 등록되었습니다.");
                        Sleep(300);
                    } else printf(" %-50s", "등록을 취소합니다.");//등록도중 ESC
                } else printf(" %-50s", "-최대인원수를 초과했습니다.");

                key = 0;//입력키를 초기화합니다.
                Sleep(500);
            }
                break;

            case 2: //수정부분
            {
                if (count == 0) //갯수가 0이라면
                {
                    printf(" -등록된 회원이 없습니다.");
                    Sleep(400);
                    break;
                }

                temp = updata_members(members, count);
                //수정(메뉴선택)실행
                if (temp == -3) {
                    //일치하는 식별번호가 없다면
                    printf("\n\n-%-60s", "일치하는 값이 없습니다.");
                    Sleep(800);
                } else if (temp == 0) {
                    //취소
                    printf(" %-50s", "수정을 취소합니다.");
                    Sleep(500);
                    break;
                } else {
                    //등록(수정)합니다. temp는 수정하고싶은 번호.
                    yes = add_members(members, temp - 1, count);
                    if (yes == 1) {
                        printf(" %-50s", "회원정보가 정상적으로 수정되었습니다.");
                        Sleep(300);
                    }
                        //그와중에 ESC를 눌렀다면
                    else printf(" %-50s", "수정을 취소합니다.");
                    Sleep(500);
                }
            }
                break;

            case 3: {//ESC 0 , 삭제완료 count값, 검색실패 -3
                if (count == 0) {
                    printf(" -등록된 회원이 없습니다.");
                    Sleep(400);
                    break;
                }

                temp = delete_members(members, count);
                //삭제(메뉴선택)실행
                if (temp == -3) {
                    printf("\n\n-%-60s", "일치하는 값이 없습니다.");
                    Sleep(300);
                } else if (temp == -1) printf("%-50s", "삭제를 취소합니다.");
                else {

                    printf("%-52s", "삭제가 완료되었습니다.");
                    for (i = temp; i < count - 1; i++)
                        members[i] = members[i + 1];
                    count--;
                    Sleep(500);
                }
                Sleep(500);


            }
                break;

                //회원출력, 출력함수안에 정렬과 검색이 포함되어있습니다.
            case 4:
                print_members(members, count);
                break;

            case 6: {    //관리자 실행, 관리자함수 안에 암호변경과 파일출력이 포함되어있습니다.
                supervisor(members, count);

            }
                break;
            case 7: {
                //종료시 화면
                gotoxy(0, 0);
                printf("┌[ 회원관리 ]───────────────────────────────┐\n");
                printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
                printf("│                                                                          │\n");
                printf("│ [Enter] 예     [Anykey] 싫어                                             │\n");
                printf("├─────┬───────────────────────────────┘\n");
                printf("│   종료   │ 정말 종료하시겠습니까?");
                printf("\n└─────┘\n");
                key = getch();
                //Enter을 입력하면 Key=7(while종료값)으로.
                if (key == 13) key = 7;
            }
                //그외 기타입력은 전부 무시합니다.
            default :
                break;
        }


        //데이터파일 갱신 (메뉴를 한번 실행할때마다)
        fp = fopen(DATAFILE, "wb");

        fwrite(&filecntl, sizeof(HEAD), 1, fp);

        for (i = 0; i < count; i++)
            fwrite(&members[i], sizeof(PERSON), 1, fp);
        fclose(fp);
    }
    //종료되었을 때  화면
    system("cls");
    printf("                          ─ 프로그램이 종료됩니다. ─\n\n 사용해주셔서 감사합니다. <[-_- <]");
    Sleep(1000);
    return 0;
}

int supervisor(PERSON members[], int count) {
    POS mespos = {0, 0}, stpos = {0, 0}, repos = {0, 0};//각종 좌표저장용
    STICK cntl = {NULL, NULL}, none = {NULL, NULL};//텍스트 (나만의 scanf)
    int key = 0, len = 0, i = 0, yes = -1, textlen = 10;//각종 변수들
    char string[30] = {0,};//문자열 공간

    while (key != 27) {
        Sleep(30);  //이 역시 화면깜빡임을 줄일려고 잠깐 딜레이
        system("cls");// 화면을 지우고, 딜레이중에 입력된 키를 다버립니다.
        while (kbhit()) getch();

        gotoxy(0, 0);
        printf("┌[ 회원관리 ]───────────────────────────────┐\n");
        printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
        printf("│                                                                          │\n");
        printf("│   [F1] 회원명단 출력   [F2] 암호변경   [Esc] 이전                        │\n");
        printf("├─────┬───────────────────────────────┘\n");
        printf("│ 관 리 자 │");
        repos = wherexy();
        printf("\n└─────┘\n");
        stpos = wherexy();

        do { textwindow(mespos, NULL, &key, none, NONE, &len); } while (key != 1 && key != 2 && key != 27);

        switch (key) {
            case 1 ://회원명단출력
            {

                if (count == 0) {
                    gotoxy(repos.x, repos.y);
                    printf("등록된 회원이 없습니다.");
                    Sleep(400);
                } else {
                    key = printfile(members, count);
                    if (key != 0) key = 27;
                }

            }
                break;

            case 2 : {//비밀번호 변경
                gotoxy(stpos.x, stpos.y);
                printf(" ┌───────────────┐\n");
                printf(" │기존비밀번호 : ");
                mespos = wherexy();
                printf("               │");
                printf("\n └───────────────┘\n");

                string[0] = '*';
                do { cntl = textwindow(mespos, string, &key, none, NONE, &textlen); } while (key != 27 && key != 13);
                sticktostr(cntl, string);

                if (key == 27) break;
                else if (strcmp(string, filecntl.pw) == 0) {
                    gotoxy(stpos.x, stpos.y);
                    printf(" ┌───────────────┐\n");
                    printf(" │변경비밀번호 : ");
                    mespos = wherexy();
                    printf("               │");
                    printf("\n └───────────────┘\n");

                    do { cntl = textwindow(mespos, NULL, &key, none, NONE, &textlen); } while (key != 27 && key != 13);
                    if (key == 27) break;
                    else {
                        sticktostr(cntl, string);
                        strcpy(filecntl.pw, string);
                        gotoxy(repos.x, repos.y);
                        printf("변경이 완료되었습니다.");
                        Sleep(1000);
                        key = 27;
                    }
                } else {
                    gotoxy(repos.x, repos.y);
                    printf("[ERROR] 잘못된 비밀번호입니다.");
                    Sleep(600);
                }
            }
                break;
            default :
                break;
        }
    }
    return 0;
}

int printfile(PERSON members[], int count) {//회원명단 파일출력
    POS stpos = {0, 0}, mespos = {0, 0};
    int key = 0, len = 0, i = 0, yes = 0;
    STICK cntl = {NULL, NULL};
    char string[20];
    FILE *fp = NULL;

    if (count == 0) return 0;

    gotoxy(0, 0);
    printf("┌[ 회원관리 ]───────────────────────────────┐\n");
    printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
    printf("│                                                                          │\n");
    printf("│   [Esc] 이전                                                             │\n");
    printf("├─────┬───────────────────────────────┘\n");
    printf("│ 파일출력 │");
    stpos = wherexy();
    printf("\n└─────┘\n");
    printf(" ┌────────────┐\n");
    printf(" │파일명 : ");
    mespos = wherexy();
    printf("               │");
    printf("\n └────────────┘");

    while (yes == 0 && key != 27) {
        do { cntl = textwindow(mespos, NULL, &key, cntl, FILENAME, &len); } while (key != 27 && key != 13);

        switch (key) {
            case 13 : {
                gotoxy(stpos.x, stpos.y);
                sticktostr(cntl, string);
                printf("[%s] 파일을 생성하실려면 아무키나 누르세요[취소 ESC]", string);
                key = getch();

                if (key != 27) {
                    fp = fopen(string, "wt");
                    if (fp == NULL) {
                        gotoxy(stpos.x, stpos.y);
                        printf("&60s", "[ERROR] 잘못된 파일명입니다. 다시입력해주세요");
                        key = 0;
                    } else yes = 1;
                }

            }
                break;
            default :
                break;
        }
    }


    //출력파일 제일 윗부분(머리글행)
    fprintf(fp, PRINTHEAD);
    for (i = 0; i < count; i++)
        fprintf(fp, "[%d] %12s %12s %12s    %c학년\n", i + 1, members[i].id, members[i].name, members[i].phone,
                members[i].grade);

    fclose(fp);

    if (yes == 0) {
        gotoxy(stpos.x, stpos.y);
        printf("%-64s", "취소됩니다.");
    } else {
        gotoxy(stpos.x, stpos.y);
        printf("%-64s", "출력이 완료되었습니다.");
    }

    Sleep(500);
    return yes;
}


int delete_members(PERSON members[], int count) {//삭제함수, 함수내에서 삭제여부와 무엇을 삭제할지선택합니다.

//변수설명은 윗부분과 동일하기에(..) 생략합니다.
    POS mespos = {0, 0}, stpos = {0, 0}, repos = {0, 0};
    STICK cntl = {NULL, NULL};
    int key = 0, len = 3, del = 0, oh = 0, i = 0, yes = -1;
    char string[30];

    while (key != 27 && key != 13) {

        gotoxy(0, 0);
        printf("┌[ 회원관리 ]───────────────────────────────┐\n");
        printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
        printf("│                                                                          │\n");
        printf("│   [F1] 목록   [Esc] 이전                                                 │\n");
        printf("├─────┬───────────────────────────────┘\n");
        printf("│ 회원삭제 │");
        repos = wherexy();
        printf("식별번호 : ");
        mespos = wherexy();
        printf("\n└─────┘\n");
        stpos = wherexy();

        do { cntl = textwindow(mespos, NULL, &key, cntl, ID, &len); } while (key != 1 && key != 27 && key != 13);

        switch (key) {
            //회원목록
            case 1:
                print_members(members, count);
                break;
            case 13://엔터를 입력했을때
            {
                //받을 글자를
                sticktostr(cntl, string);
                del = atoi(string);//atoi( A to int)로 숫자로 변환합니다.
                //stdlib 헤더파일에 존재합니다.

                //숫자를 가지고 파일의 식별번호와 비교합니다.
                for (i = 0; i < count; i++)
                    if (del == members[i].count) {
                        yes = 1;//찾았다면 1
                        break; //탈-출
                    }
                if (yes == -1) yes = -3;    //그외 못찾았다면 -3
            }
                break;
                //그외 잡입력은 가볍게 무시해줍니다.
            default :
                break;
        }
    }

    if (yes == 1) {    //찾았다면!
        gotoxy(repos.x, repos.y);
        printf("%-52s", " 삭제를 완료 하실려면 아무키나 누르세요 [취소 ESC]");
        gotoxy(stpos.x, stpos.y);
        printf(">> [%05d]%12s %12s %12s    %c학년 <<\n", members[i].count, members[i].id, members[i].name,
               members[i].phone, members[i].grade);
        printf(" ┌───────────────────────────┐\n");

        key = getch();
        if (key == 27) yes = -1;
        else {
            yes = i;//찾은 count를 yes에게주고
            filecntl.delsize++;//삭제를 성공했으므로 삭제갯수를 추가합니다.
            //이 역시 휴지통기능을 넣을려다가 시간부족으로 무산되어서 쓸모없는 변수입니다.
        }
    }
    gotoxy(repos.x, repos.y);
    return yes; //yes를 반환합니다.
}


int updata_members(PERSON members[], int count) {//수정함수 함수내에서 수정여부와 수정을 위해 필요한 변수를 반환합니다.
    //자료형은 위와 동일
    POS mespos = {0, 0}, stpos = {0, 0};
    STICK cntl = {NULL, NULL};
    int key = 0, len = 3, remake = 0, oh = 0, i = 0, yes = 0;
    char string[30];

    while (key != 27 && key != 13) {
        Sleep(30);
        while (kbhit()) getch();
        system("CLS");
        //손가락이 아파옵니다.. 위(삭제함수)와 동일합니다!! 교수님 사랑합니다(?)
        gotoxy(0, 0);
        printf("┌[ 회원관리 ]───────────────────────────────┐\n");
        printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
        printf("│                                                                          │\n");
        printf("│   [F1] 목록   [Esc] 이전                                                 │\n");
        printf("├─────┬───────────────────────────────┘\n");
        printf("│ 회원수정 │");
        stpos = wherexy();
        printf("식별번호 : ");
        mespos = wherexy();
        printf("\n└─────┘\n");

        do { cntl = textwindow(mespos, NULL, &key, cntl, ID, &len); } while (key != 1 && key != 27 && key != 13);

        switch (key) {
            case 1:
                print_members(members, count);
                break;
            case 13: {
                sticktostr(cntl, string);
                remake = atoi(string);

                for (i = 0; i < count; i++)
                    if (remake == members[i].count) yes = 1;
                if (yes == 0) yes = -3;
            }
                break;
            default :
                break;
        }
    }
//	printf("remake = %d",remake);
//	Sleep(2000);
    if (yes == 0) gotoxy(stpos.x, stpos.y);
    else if (yes == 1) yes = remake;

    return yes;
}


int sticktostr(STICK cntl, char *const string) {//텍스트 연결리스트(나만의 scanf)를 보기좋게 문자열로 바꿔줍니다.
    int i = 0;
    for (i = 0; cntl.head != NULL; i++) {
        string[i] = cntl.head->data;
        cntl.head = cntl.head->next;
    }
    string[i] = '\0';

    return i;
}

int array_members(PERSON members[], int count) {//회원을 정렬합니다.
    int i = 0, j = 0, data = 0;
    PERSON temp;
    POS mespos = {0, 0};

    gotoxy(0, 0);
    printf("┌[ 회원관리 ]───────────────────────────────┐\n");
    printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
    printf("│                                                                          │\n");
    printf("│   [1] 학번   [2] 이름    [Esc] 이전                                      │\n");
    printf("├─────┬───────────────────────────────┘\n");
    printf("│ 회원정렬 │");
    mespos = wherexy();
    printf("\n└─────┘\n");
    printf(" ├[00000]%12s %12s %12s   %6s┤\n", "학번", "이름 ", "전화번호", "학년");
    printf(" ├───────────────────────────┤\n");

    //F1을 연달아 입력하여 그냥 넘어가는 경우가 많아서
    //정렬할때만 F1 - 숫자 입력으로 바꿨습니다.
    while (data != 27 && data != '1' && data != '2') {
        data = getch();
        switch (data) {
            case '1': {//학번순 버블정렬.
                for (i = 1; i < count; i++)
                    for (j = 0; j < count - i; j++)
                        if (strcmp(members[j].id, members[j + 1].id) > 0) {
                            temp = members[j];
                            members[j] = members[j + 1];
                            members[j + 1] = temp;
                        }
                gotoxy(mespos.x, mespos.y);
                printf("학번 순으로 정렬이 완료되었습니다.");
                Sleep(500);
            }
                break;
            case '2': {//이름순 버블정렬.
                for (i = 1; i < count; i++)
                    for (j = 0; j < count - i; j++)
                        if (strcmp(members[j].name, members[j + 1].name) > 0) {
                            temp = members[j];
                            members[j] = members[j + 1];
                            members[j + 1] = temp;
                        }
                gotoxy(mespos.x, mespos.y);
                printf("이름 순으로 정렬이 완료되었습니다.");
                Sleep(500);
            }
                break;

                //그외는 신나게 무시해줍니다.
            default :
                break;
        }
    }
    return 0;
}

int search_members(PERSON members[], int count) {//검색함수
    int i = 0, j = 0, data = 0, key = 0;
    int len[2] = {10, 12}, yes = 0;
    char string[20] = {0,};
    STICK cntl0 = {NULL, NULL}, cntl1 = {NULL, NULL};
    PERSON temp;
    POS mespos = {0, 0}, stpos = {0, 0}, nowpos = {0, 0};

    Sleep(30);
    system("cls");
    gotoxy(0, 0);
    printf("┌[ 회원관리 ]───────────────────────────────┐\n");
    printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
    printf("│                                                                          │\n");
    printf("│   [1] 학번   [2] 이름    [Esc] 이전                                      │\n");
    printf("├─────┬───────────────────────────────┘\n");
    printf("│ 회원검색 │");
    mespos = wherexy();
    printf("\n└─────┘\n");
    printf(" ├[00000]%12s %12s %12s   %6s┤\n", "학번", "이름 ", "전화번호", "학년");
    printf(" ├───────────────────────────┤\n");
    stpos = wherexy();
    printf(" └───────────────────────────┘");

    //이 역시 마찬가지로
    //F1을 연달아 입력하는경우가 많아 숫자로.
    while (data != 27 && data != '1' && data != '2') {
        data = getch();

        switch (data) {
            case '1': {
                gotoxy(mespos.x, mespos.y);
                printf("입력 : ");
                nowpos = wherexy();
                do { cntl0 = textwindow(nowpos, NULL, &key, cntl0, ID, &len[0]); } while (key != 27 && key != 13);
                if (key == 27) break;//ESC 입력시 종료
                sticktostr(cntl0, string);
                strcpy(members[count].id, string);

                gotoxy(stpos.x, stpos.y);
                for (i = 0; i < count; i++)
                    if (strcmp(members[i].id, string) == 0) {
                        yes = 1;//회원을 출력했다면 yes에 1을 넣어줍니다.
                        printf(" │[%05d] %12s %12s %12s   %c학년│\n", members[i].count, members[i].id, members[i].name,
                               members[i].phone, members[i].grade);
                    }
                printf(" └───────────────────────────┘");
            }
                break;

            case '2': {
                gotoxy(mespos.x, mespos.y);
                printf("입력 : ");
                nowpos = wherexy();
                do { cntl1 = textwindow(nowpos, NULL, &key, cntl1, NONE, &len[1]); } while (key != 27 && key != 13);
                if (key == 27) break; //ESC 입력시 종료
                sticktostr(cntl1, string);
                strcpy(members[count].name, string);

                gotoxy(stpos.x, stpos.y);
                for (i = 0; i < count; i++)
                    if (strcmp(members[i].name, string) == 0) {
                        yes = 1;//회원을 출력했다면 yes에 1을 넣어줍니다.
                        printf(" │[%05d]%12s %12s %12s    %c학년│\n", members[i].count, members[i].id, members[i].name,
                               members[i].phone, members[i].grade);
                    }
                printf(" └───────────────────────────┘");
            }
                break;

            default :
                break;
        }
    }
    gotoxy(mespos.x, mespos.y);
    if (key != 27 && data != 27) {

        if (yes == 1) {//회원을 출력했다면
            printf("검색이 완료되었습니다. [아무키] 이전");
            getch();
        } else if (yes == 0) {//안했다면
            printf("검색결과가 존재하지 않습니다. [아무키] 이전");
            getch();
        }
    }
    return 0;
}

int print_members(PERSON members[], int count) {//회원 목록함수
    int i;
    POS mespos = {0, 0};
    POS stpos = {0, 0};
    int key = 0, len = 0;
    STICK cntl = {NULL, NULL};

    if (count > 0)//보여줄 회원이 있는지 확인합니다.
    {
        //딜레이 도중 잡입력은 다버립니다.
        while (kbhit()) getch();

        while (key != 27) {
            gotoxy(0, 0);
            printf("┌[ 회원관리 ]───────────────────────────────┐\n");
            printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
            printf("│                                                                          │\n");
            printf("│   [F1] 검색   [F2] 정렬   [Esc] 이전                                     │\n");
            printf("├─────┬───────────────────────────────┘\n");
            printf("│ 회원목록 │");
            mespos = wherexy();
            printf("%52s", " ");
            printf("\n└─────┘\n");
            printf(" ├[00000]%12s %12s %12s   %6s┤\n", "학번", "이름 ", "전화번호", "학년");
            printf(" ├───────────────────────────┤\n");
            stpos = wherexy();
            printf(" └───────────────────────────┘");

            gotoxy(stpos.x, stpos.y);
            for (i = 0; i < count; i++)
                printf(" │[%05d]%12s %12s %12s    %c학년│\n", members[i].count, members[i].id, members[i].name,
                       members[i].phone, members[i].grade);
            printf(" └───────────────────────────┘");

            do { textwindow(stpos, NULL, &key, cntl, NONE, &len); } while (key != 1 && key != 2 && key != 27);

            switch (key) {
                //검색
                case 1:
                    search_members(members, count);
                    break;
                    //정렬
                case 2:
                    array_members(members, count);
                    break;
                    //ESC
                case 27:
                    break;
                default :
                    break;
            }
        }
    } else {
        printf(" -등록된 회원이 없습니다.");
        Sleep(400);
    }

    return 0;
}

int add_members(PERSON members[], int count, int remake) {//회원 등록함수.(수정포함) 유일하게 main이 아닌 함수 내에서 회원데이터를 건드립니다.
//수정시에는 전체갯수와 수정을 원하는 번호가 필요하기에 remake와 count를 다르게씁니다.

    POS stpos = {0, 0}, mespos = {0, 0};
    int i = 0, key = 0, len[4] = {10, 12, 1, 1}, yes = 0, t = 0, ins;
    char string[30] = {0,};
    STICK cntl0 = {NULL, NULL}, cntl1 = {NULL, NULL}, cntl2 = {NULL, NULL}, cntl3 = {NULL, NULL};
    int check = 0;

    fflush(stdin);
    while (kbhit()) getch();

    gotoxy(0, 0);
    printf("┌[ 회원관리 ]───────────────────────────────┐\n");
    printf("│                                    회원수 [%3d] / 최대 회원수 [%3d]      │\n", count, N);
    printf("│                                                                          │\n");
    printf("│  [Tap] 항목이동   [Esc] 취소                                             │\n");
    printf("├─────┬───────────────────────────────┘\n");
    if (remake >= 1)printf("│ 회원수정 │");
    else printf("│ 회원등록 │");
    mespos = wherexy();
    printf("\n└─────┘\n");

    if (remake >= 1) {
        printf(" ┌[#%05d]──────────────────┐", members[count].count);
        stpos = wherexy(); //수정하슈
        gotoxy(stpos.x - 2, stpos.y + 1);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 2);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 3);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 4);
        printf("│");
        gotoxy(stpos.x, stpos.y);
        printf(" \n │학  번   : ");
        stpos = wherexy();
        printf("                ←[%12s]", members[count].id);
        printf("\n │이  름   :                 ←[%12s]\n │전화번호 :                 ←[%12s]\n │학  년   :  학년           ←[%7c 학년]",
               members[count].name, members[count].phone, members[count].grade);
        printf(" \n └──────────────────────┘\n");

    } else {
        printf(" ┌[#%05d]─────────┐", filecntl.size);
        stpos = wherexy(); //수정하슈
        gotoxy(stpos.x - 2, stpos.y + 1);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 2);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 3);
        printf("│");
        gotoxy(stpos.x - 2, stpos.y + 4);
        printf("│");
        gotoxy(stpos.x, stpos.y);
        printf(" \n │학  번   : ");
        stpos = wherexy();
        printf("\n │이  름   : \n │전화번호 :    -    -\n │학  년   :  학년");
        printf(" \n └─────────────┘\n");
    }
    while (key != 27 && key != 13) {
        gotoxy(stpos.x, stpos.y);
        if (check <= 1) {
            check = 0;
            do { cntl0 = textwindow(stpos, NULL, &key, cntl0, ID, &len[0]); }
            while (key != 27 && key != 9 && key != 13);
            if (key == 27) break;


        }
        putch('\n');

        (stpos.y)++;
        if (check <= 2) {
            check = 0;
            do { cntl1 = textwindow(stpos, NULL, &key, cntl1, NONE, &len[1]); }
            while (key != 27 && key != 9 && key != 13);
            if (key == 27) break;

        }
        putch('\n');

        (stpos.y)++;

        if (check <= 3) {
            check = 0;
            do { cntl2 = textwindow(stpos, NULL, &key, cntl2, PHONE, &len[2]); }
            while (key != 27 && key != 9 && key != 13);
            if (key == 27) break;

        }
        putch('\n');


        (stpos.y)++;
        if (check <= 4) {
            check = 0;
            do { cntl3 = textwindow(stpos, NULL, &key, cntl3, GRADE, &len[3]); }
            while (key != 27 && key != 9 && key != 13);
            if (key == 27) break;

        }
        if (key == 9) stpos.y = stpos.y - 3;
        else if (key == 13) {
            key = 0;
            gotoxy(mespos.x, mespos.y);
            //ID 입력검사
            if (cntl0.head == NULL) {
                check = 1;
                printf("%-50s", " [ERROR] 학번(숫자) 을 입력해주세요.");
            }
                //이름 입력검사
            else if (cntl1.head == NULL) {
                check = 2;
                printf("%-50s", " [ERROR] 이름을 입력해주세요.");
            }
                //전번 입력검사
//			 else if(cntl2.head==NULL){check=3; printf("%-50s"," [ERROR] 전화번호(숫자) 를 입력해주세요."); }
                //학년 입력검사
            else if (cntl3.head == NULL) {
                check = 4;
                printf("%-50s", " [ERROR] 학년(1~4) 을 입력해주세요.");
            }
                //정상입력
            else {
                //ID 중복검사
                sticktostr(cntl0, string);
                if (remake == 0) {
                    for (i = 0; i < count; i++)
                        if (strcmp(members[i].id, string) == 0) {
                            check = 1;
                            printf("%-50s", " [ERROR] 중복된 학번입니다. 다시 입력해주세요.");
                            break;
                        }
                } else {
                    for (i = 0; i < remake; i++)
                        if (i != count && strcmp(members[i].id, string) == 0) {
                            check = 1;
                            printf("%-50s", " [ERROR] 중복된 학번입니다. 다시 수정해주세요.");
                            break;
                        }
                }

                //문제가 없다면
                if (check == 0) {
                    if (remake == 0) printf("%-52s", " 등록을 완료하실려면 아무키나 누르세요 [재입력 ESC]\n");
                    else printf("%-52s", " 수정을 완료하실려면 아무키나 누르세요 [재입력 ESC]\n");

                    fflush(stdin);
                    key = getch();

                    if (key != 27) key = 13;
                    else {
                        gotoxy(mespos.x, mespos.y);
                        printf("%52s", " ");
                        key = 0;
                    }
                }
            }

            stpos.y = stpos.y - 3;

        }
    }
    if (key == 27) yes = 2;
    else yes = 1;

    if (yes == 1) {

        //저장부분

        sticktostr(cntl0, string);
        strcpy(members[count].id, string);
        sticktostr(cntl1, string);
        strcpy(members[count].name, string);
        sticktostr(cntl2, string);
        strcpy(members[count].phone, string);
        sticktostr(cntl3, string);
        members[count].grade = string[0];

        if (!(remake >= 1)) //수정이 아닐시에만
        {
            members[count].count = filecntl.size;
            (filecntl.size)++;
        }

    }
    gotoxy(mespos.x, mespos.y);

    return yes;

}
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////이밑으로는 전부 나만의scanf 관련 함수들입니다.////////////////////////////

//텍스트          시작 좌표       특정 작업할때   입력키    텍스트    메뉴값    길이*ins값
STICK textwindow(const POS stpos, char *string, int *key, STICK cntl, USE menu, int *len) {
    //getch()로 텍스트를 하나하나 입력받아 연결리스트로 만듭니다.
    //입력하다가 특수키(Enter, F1 등등)을 입력하면 함수는 텍스트를 가진 연결리스트를 반환합니다.
    //입력키와 길이값, 특정 작업할때의 문자열도 반환하기위해서 주소전달(포인터변수)로 사용했습니다.

    STICK windo;//화면출력용 연결리스트(임시저장용)
    int max = 0, cur = 0;//최대커서, 현재커서
    int data = 0, exdata = 0;//값, 추가값
    int ins = 1;//인설트키 여부 (1, -1), 메뉴값에 곱해서 반환
    int i = 0, j = 0, yes = 0;//각종 변수들

    *key = 0;//입력키 초기화(혹시나 해서)

    if (*len < 0)//인설트 읽기
    {
        (*len) *= -1;
        ins = -1;

    }
    // 최대길이 초기화
    if (menu == PHONE) *len = 11;
    else if (menu == GRADE) *len = 1;
    else if (menu == FILENAME) *len = 13;

    // 기본입력값(FILENAME 일경우)
    if (menu == FILENAME && cntl.head == NULL) {
        cntl = addnode(cntl, '.');
        cntl = addnode(cntl, 't');
        cntl = addnode(cntl, 'x');
        cntl = addnode(cntl, 't');
        cntl.cur = NULL;
    }

    //입력키가 0일때 (특수키가 아닐때) 반복합니다.
    while (*key == 0) {
        //반응속도조절
        Sleep(30);
        //커서 형태
        if (*len == 0) cursor_type(NO);
        else if (ins == 1) cursor_type(NORMAL);
        else cursor_type(SOLID);
        gotoxy(stpos.x, stpos.y);

        //화면출력용 연결리스트(임시저장)
        windo.cur = cntl.cur;
        windo.head = cntl.head;

        //메뉴별 화면출력
        switch (menu) {
            //학번, 파일이름, 분류없음은 그냥 화면에 글자를 보여줍니다.
            case ID :
            case FILENAME :
            case NONE : {
                gotoxy(stpos.x, stpos.y);
                for (i = 0; windo.head != NULL; i++) {
                    if (string != NULL) {
                        if (string[0] = '*') putch('*');
                    } else putch(windo.head->data);
                    windo.head = windo.head->next;
                }
                //삭제(빈공간 당기기)
                gotoxy(stpos.x + i, stpos.y);
                putch(' ');
            }
                break;

                //성적일 경우에는 끝에 "학년" 문자열을 붙여줍니다.
            case GRADE: {
                gotoxy(stpos.x, stpos.y);
                if (windo.head != NULL) putch(windo.head->data);
                printf(" 학년 ");
            }
                break;

                //전화번호일때는 '-' 을 보여줍니다. (000-0000-0000)
            case PHONE : {
                gotoxy(stpos.x + 3, stpos.y);
                putch('-');
                gotoxy(stpos.x + 8, stpos.y);
                putch('-');

                gotoxy(stpos.x, stpos.y);
                for (i = 0; windo.head != NULL; i++) {
                    if (i == 3 || i == 8) {
                        i++;
                        putch('-');
                    }
                    putch(windo.head->data);
                    windo.head = windo.head->next;
                }
                //삭제(빈공간 당기기)

                gotoxy(stpos.x + i, stpos.y);
                putch(' ');
                putch(' ');
                gotoxy(stpos.x + 3, stpos.y);
                putch('-');
                gotoxy(stpos.x + 8, stpos.y);
                putch('-');

            }
                break;

                //그외 잘못된 메뉴값이 들어왔을때는 종료합니다.
            default:
                printf("textwindow- menu ERROR 01");
                exit(-1);
                system("CLS");
                break;
        }

        /////연결리스트의 커서를 정수값으로 바꿉니다.
        //(글자수, cur= 현재위치, max= 최대값)
        cur = wherecur(cntl, CUR);
        max = wherecur(cntl, MAX);

        //전화번호일경우에는, '-'을 생각해서 커서를 조작해줍니다.
        if (menu == PHONE) {
            if (cur >= 3) cur = cur + 1;
            if (cur >= 8) cur = cur + 1;
        }

        //정수값으로 바뀐 커서값을 이용해 커서를 조작합니다.
        gotoxy(stpos.x + cur, stpos.y);



        //데이터 입력 (글자)
        data = getch();

        yes = 1;

        //입력글자 제한(   글자		      특문2	      \b		  \r		cntl+x)
        if (menu == NONE || menu == FILENAME) {
            if ((data > 127 || data < 32) && data != 0 && data != 8 && data != 13 && data != 9 && data != 24 &&
                data != 27 && data != 224) {
                yes = 0;
                fflush(stdin);
            }

            if (menu == FILENAME) {                                                    //   "										  '\'
                if (data == '*' || data == '|' || data == '?' || data == '/' || data == ':' || data == 34 ||
                    data == '<' || data == '>' || data == '+' || data == 92) {
                    yes = 0;
                    fflush(stdin);
                }
            }
        } else if (menu == PHONE || menu == ID) {
            if ((data < 48 || data > 57) && data != 32 && data != 0 && data != 8 && data != 13 && data != 9 &&
                data != 24 && data != 27 && data != 224) {
                yes = 0;
                fflush(stdin);
            }
            if (menu == ID && data == 32) {
                yes = 0;
                fflush(stdin);
            }
        } else if (menu == GRADE) {
            if ((data < 49 || data > 52) && data != 0 && data != 8 && data != 13 && data != 24 && data != 9 &&
                data != 27 && data != 224) {
                yes = 0;
                fflush(stdin);
            }
        }

        ///입력글자 판단
        if (yes)
            switch (data) {
                default : {    //최대크기 초과
                    if (*len == 1) {//1글자일땐 귀찮으니 알아서 지우고 재입력해줍니다.
                        cntl = delnode(cntl, 1);
                        cntl = addnode(cntl, data);
                    } else if (max >= (*len)) {
                        //인설트키(수정)이 켜졌을때
                        //전화번호일때만 최대글자를 2글자 추가해줍니다.('-' 생각)
                        if (menu == PHONE && ins < 0 && (max + 2) > cur) {
                            cntl = delnode(cntl, -1);
                            cntl = addnode(cntl, data);
                        }
                            //ins가 켜져있고, 마지막 커서가 아닐경우
                        else if (ins < 0 && max > cur) {
                            cntl = delnode(cntl, -1);
                            cntl = addnode(cntl, data);
                        }
                            //그외
                        else break;
                    }
                        //정상일때 ins가 꺼져있을경우
                    else if (ins > 0) cntl = addnode(cntl, data);
                        //정상일때 ins가 켜져있을경우
                    else {
                        cntl = delnode(cntl, -1);
                        cntl = addnode(cntl, data);
                    }
                }
                    break;
                    //일반키\b 8 / \r 13 / space 32 / Tap 9 / Esc 27 / cntl+ x 24
                case '\r':
                    *key = 13;
                    break;
                case '\b':
                    cntl = delnode(cntl, 1);
                    break;

                case 9 :
                    *key = 9;
                    break;
                case 27 :
                    *key = 27;
                    break;
                case 24 :
                    *key = 24;
                    break;

                    //특수키는 아스키코드로 표현불가능, (2바이트) 이므로 2번읽어줍니다.

                    //특수키(  0-**)F1 59 / F2 60 / F3 61 / F4 62 /... F10 68 /27 ESC
                case 0 : {
                    exdata = getch();
                    switch (exdata) {
                        default :
                            break;
                        case 59 :
                            *key = 1;
                            break;
                        case 60 :
                            *key = 2;
                            break;
                        case 61 :
                            *key = 3;
                            break;
                        case 62 :
                            *key = 4;
                            break;
//							case 63 : *key=5; break; //F5는 쓰지않습니다.
                        case 64 :
                            *key = 6;
                            break;
                        case 65 :
                            *key = 7;
                            break;

                            //F8이상은 다른 특수키와 아스키 코드가 겹칠수있으므로
                            //F8 = 8 로 표현이 불가능합니다.
                    }
                }
                    break;

                    //특수키( 224-**)◀75 / ▶77 / HOME 71 / END 79/ DEL 83 / INS 82 / F11 133 / F12 134
                case 224 : {
                    exdata = getch();
                    switch (exdata) {
                        case 71 :
                            cntl.cur = NULL;
                            break;
                            //Home 를 입력했을경우 커서를 제일앞으로 이동합니다.

                        case 79 :
                            //End 키를 입력했을경우, 값이 있는지 판단하고 커서를 제일 뒤값으로 이동합니다.
                            if (cntl.head == NULL) break;
                            else {
                                windo.head = cntl.head;

                                while (windo.head->next != NULL)
                                    windo.head = windo.head->next;

                                cntl.cur = windo.head;

                            }
                            break;

                            //키보드 왼쪽키를 눌렀을경우, 시작부분인지 판단하고 커서를 이동합니다.
                        case 75 :
                            if (cntl.cur == NULL || cntl.head == NULL) break;
                            else cntl.cur = cntl.cur->prev;
                            break;
                            //키보드 오른쪽키를 눌렀을경우, 끝부분인지 판단하고 커서를 이동합니다.

                        case 77 :
                            if (cntl.head == NULL) break;//****오류주의 이거때문에 3시간을 날림
                            else if (cntl.cur == NULL) cntl.cur = cntl.head;
                            else if (cntl.cur->next == NULL)break;
                            else cntl.cur = cntl.cur->next;
                            break;

                            //insert키를 눌렀을경우, ins의 부호를 바꿉니다.
                        case 82 :
                            ins *= (-1);
                            break;
                            //Del를 눌렀을경우 반대방향으로 지웁니다.
                        case 83 :
                            cntl = delnode(cntl, -1);
                            break;
                        default :
                            break;
                    }
                }
                    break;

            }//while-switch
    }//while

    if (*len != 0) (*len) *= ins; //인설트값 반환
    cursor_type(NO);//커서형태 : 없음
    return cntl;//텍스트 연결리스트 반환
}


STICK addnode(STICK cntl, char item) {
    NODE *temp;//잠깐 저장할 빈공간
    NODE *save;//추가할 연결리스트

    //1. 첫번째 입력
    if (cntl.head == NULL) {
        cntl.head = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.head == NULL) {
            printf("ERROR add-01\n");
            exit(-1);
        }

        cntl.head->next = NULL;
        cntl.head->prev = NULL;
        cntl.head->data = item;

        cntl.cur = cntl.head; //커서이동
    }

        //2. 첫번째 입력이 아닌데 커서가 제일 앞
    else if (cntl.cur == NULL) {
        cntl.cur = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.cur == NULL) {
            printf("ERROR add-02\n");
            exit(-1);
        }

        cntl.cur->next = cntl.head;
        cntl.cur->prev = NULL;
        cntl.cur->data = item;

        cntl.head = cntl.cur;// 헤드를 한칸 당겼기 때문에, 결과적으로 커서가 이동
    }

        //3. 마지막 입력
    else if (cntl.cur->next == NULL) {
        cntl.cur->next = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.cur == NULL) {
            printf("ERROR add-03\n");
            exit(-1);
        }

        temp = cntl.cur;

        cntl.cur = cntl.cur->next; //커서이동

        cntl.cur->next = NULL;
        cntl.cur->prev = temp;
        cntl.cur->data = item;

    }
        //4. 중간입력
    else {
        save = (NODE *) calloc(1, sizeof(NODE));
        if (save == NULL) {
            printf("ERROR add-04\n");
            exit(-1);
        }

        save->next = cntl.cur->next;
        save->prev = cntl.cur;
        save->data = item;

        temp = cntl.cur->next;
        temp->prev = save;

        cntl.cur->next = save;
        cntl.cur = cntl.cur->next;//커서이동
    }

    return cntl;

}

STICK delnode(STICK cntl, int ins) {
    NODE *save;//추가할값
    NODE *temp;//버릴값(임시)

//ins  삭제방향 , 0 현재커서(커서기준 왼쪽글자) , 1 다음커서(오른쪽글자)
    switch (ins) {
        case 1: {
            //1.값이없을때
            if (cntl.head == NULL) break;

                //2.값이 있는데 커서가 제일앞일때
            else if (cntl.cur == NULL) break;

                //3.마지막삭제
            else if (cntl.cur->next == NULL) {
                temp = cntl.cur;

                //3.1 값이 한개밖에 없을때
                if (cntl.head == cntl.cur) {
                    temp = cntl.cur;
                    free(temp);
                    cntl.head = NULL;
                    cntl.cur = NULL;
                }
                    //3.2 그외
                else {
                    //커서이동
                    cntl.cur = cntl.cur->prev;
                    free(temp);
                    cntl.cur->next = NULL;
                }
            }

                //4.중간삭제
            else {
                temp = cntl.cur;
                save = cntl.cur->next;


                if (cntl.head == cntl.cur) {
                    cntl.head = save;
                    //커서이동
                    cntl.cur = NULL;
                    save->prev = NULL;
                    free(temp);
                } else {    //커서이동
                    cntl.cur = cntl.cur->prev;
                    free(temp);
                    cntl.cur->next = save;
                    save->prev = cntl.cur;
                }
            }
        }
            break;

        case -1: {
            //1.값이없을때
            if (cntl.head == NULL) break;

                //2.값이있는데 커서가 제일앞일때
            else if (cntl.cur == NULL) {
                //2-1 값이 하나밖에 없을때
                if (cntl.head->next == NULL) {
                    temp = cntl.head;
                    free(temp);
                    cntl.head = NULL;
                }
                    //2-2 그외
                else {
                    temp = cntl.head;
                    save = cntl.head->next;

                    free(temp);
                    cntl.head = save;
                }
            }

                //3.마지막삭제
            else if (cntl.cur->next == NULL) break;

                //4.중간삭제
            else {
                temp = cntl.cur->next;
                save = temp->next;
                free(temp);

                if (save == NULL) {
                    cntl.cur->next = NULL;
                } else {
                    cntl.cur->next = save;
                    save->prev = cntl.cur;
                }
            }
        }
            break;

        default:
            printf("delnode ERROR-01");
            exit(-1);
            break;
    }

    return cntl;

}

int wherecur(STICK cntl, INTCUR menu) {//커서가 어디에있는지 정수값으로 반환해줍니다.
    int i;
    switch (menu) {
        case CUR ://현재커서 찾기
            if (cntl.head == NULL || cntl.cur == NULL) i = 0;
            else if (cntl.head == cntl.cur) i = 1;
            else {
                for (i = 1; cntl.head != cntl.cur; i++)
                    cntl.head = cntl.head->next;
            }
            break;

        case MAX ://최대글자(마지막커서)찾기
            if (cntl.head == NULL) i = 0;
            else if (cntl.head->next == NULL) i = 1;
            else {
                for (i = 1; cntl.head->next != NULL; i++)
                    cntl.head = cntl.head->next;
            }
            break;

        default :
            printf("ERROR wherecur");
            exit(-1);
    }
    return i;
}

// 커서를 숨기거나 다시 표시한다.
void cursor_type(CURSOR_TYPE c) {
    CONSOLE_CURSOR_INFO CurInfo;
    //커서의 형태에 대한 정보를 가진 구조체, windows.h 에 존재합니다.
    //dwsize 글자에 채워지는 퍼센트 , bvisible 커서 on/off
    switch (c) {
        case NO://커서를 1%만 채우고 보이지않게만듭니다.
            CurInfo.dwSize = 1;
            CurInfo.bVisible = FALSE;
            break;
        case SOLID://커서를 100%채우고 보이게만듭니다.
            CurInfo.dwSize = 100;
            CurInfo.bVisible = TRUE;
            break;
        case NORMAL://커서를 20% (_) 만 채우고 보이게만듭니다.
            CurInfo.dwSize = 20;
            CurInfo.bVisible = TRUE;
            break;
    }
    //콘솔창의 커서 형태를 정하는 함수
    //콘솔 화면의 핸들(파일 디스크립터)를 가지고 조작
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void gotoxy(int x, int y) {//커서를 해당좌표로 이동시킵니다.
    COORD Cur;
    //COORD 에는 short형 x, y좌표가 있으며, 그 좌표와 화면의 핸들을 받아 위치를 옮김

    Cur.X = x;
    Cur.Y = y;
    //콘솔창의 커서를 원하는 장소로 옮길때 쓰는 함수
    //마찬가지로 콘솔화면의 핸들(파일디스크립터)를 가지고 조작합니다.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

// 커서의 x,y 좌표를 조사한다.
POS wherexy() {
    POS pos = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;
    //정확히 무슨값을 가지고있는지는 모르지만
    //현재 화면의 커서정보를 가지고있는것으로 대략적으로 알고있습니다.

    //마찬가지로 콘솔화면의 핸들을가지고 커서위치정보를 입력합니다.
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
    //얻은 좌표를 반환합니다.
    pos.x = BufInfo.dwCursorPosition.X;
    pos.y = BufInfo.dwCursorPosition.Y;

    return pos;
}

