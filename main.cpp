#include <stdio.h>
#include <stdlib.h>
#include <conio.h>//getch() ,putch(), kbhit()
#include <windows.h> //Ŀ�� �����Լ� ,system() , Sleep(�������Լ�)
#include <string.h> // strcmp(�ؽ�Ʈ ��), strcpy(�ؽ�Ʈ ����)

#define FILECHECK 2015642028//���� ������ȣ, �� ������ �´��� Ȯ���Ҷ� ���
#define N 100//��ü����
#define DATAFILE "members.dat"//���������ϸ�
#define PRINTHEAD "Made in Jiwon"//�������(���) �Ӹ���
//////////////////////////////////////
//ȸ������ ����
typedef struct members {
    int count;
    char id[12];//ID
    char name[13];//�̸�
    char phone[14];//��ȭ��ȣ
    char grade;//�г�
} PERSON;//ȸ������

typedef struct headers {
    //��ü������ ���������� ���������� ������ ���������� ������ٰ�
    //�ð��������� ����Ǿ���.. (������ �ȿ� �ִ°����� �˱����ؼ�)
    //(������ �ٷλ��������ʰ� �˻��� �ȵǰ��س��ٰ� ���߿� �����ڷ� ����)

    int filecheck;//������ȣ, ���ϻ����� ���
    int size;//����� ��ü���� (���ϰ�����ȣ)
    int delsize;//�������� (���� ������ �˱����ؼ�)
    int error;//Ʋ��Ƚ��
    char pw[12];//��й�ȣ
} HEAD;//ȸ�����


///////////////////////////
// oneline editor ������ ���� ���Ḯ��Ʈ
typedef struct nodes {
    char data;//����
    struct nodes *next;//����
    struct nodes *prev;//����
} NODE;//�ؽ�Ʈ����

typedef struct sticks {
    NODE *head;//���Ḯ��Ʈ ���
    NODE *cur;//���� Ŀ��
} STICK;//�ؽ�Ʈ ���

///////////////////////////////
//��ǥ�� ����ü�� �������ϰ� ���� ������
typedef struct pos {
    int x;
    int y;
} POS;

typedef enum {
    NONE = 1, ID, FILENAME, PHONE, GRADE
} USE; //textwindow ��
typedef enum {
    CUR = 1, MAX
} INTCUR;//wherecur ��
typedef enum {
    NO = 1, SOLID, NORMAL
} CURSOR_TYPE;//cursor_type ��
///////////////////////////////////////////////////

//#1 Ŀ�� ���� �Լ���
void cursor_type(CURSOR_TYPE c);

//Ŀ������� �ٲ���
void gotoxy(int x, int y);

//�ش���ǥ�� Ŀ���� �̵�������
POS wherexy();
//���� Ŀ����ġ�� ��ȯ����

//#2 �޴����� �Լ���
int add_members(PERSON members[], int count, int remake);

//ȸ���Է��Լ�, remake�� ������
int updata_members(PERSON members[], int count);

//����(����â)
int delete_members(PERSON members[], int count);

//����(����â)
int sticktostr(STICK cntl, char *string);
//�ؽ�Ʈ ���Ḯ��Ʈ(������ scanf���� ��ȯ��)�� ���ڿ���

int print_members(PERSON members[], int count);

//���
int search_members(PERSON members[], int count);

//�˻�
int array_members(PERSON members[], int count);
//����

int supervisor(PERSON member[], int count);

//������
int printfile(PERSON members[], int count);
//ȸ�����(����)���

//#3 ������ scanf�Լ�
STICK textwindow(const POS stpos, char *string, int *key, STICK cntl, USE menu, int *len);
//�Է� ���ڿ�	  ������ġ  ���ڿ�		  �Է�Ű	�ؽ�Ʈ���	�޴�����	�ִ����(x �μ�Ʈ����)

STICK addnode(STICK cntl, char item);

//���Ḯ��Ʈ ��� �߰��� �Լ�, item(����) ���� �� �ؽ�Ʈ ���Ḯ��Ʈ�� ��ȯ��
STICK delnode(STICK cntl, int ins);

//���Ḯ��Ʈ ��� ������ �Լ� ,���� �� �ؽ�Ʈ ���Ḯ��Ʈ�� ��ȯ��
int wherecur(STICK cntl, INTCUR menu);
//���Ḯ��Ʈ Ŀ�� ���� �Լ�( ������ġ�� x��ǥ, �� ���������� ��ȯ)

////////////////////////////////////////
//�������� - ȸ�����(�������, ������ ���� ���Ͼտ� ��ϵ�)
HEAD filecntl;


int main(int argc, char *argv[]) {
    POS stpos = {0, 0};//��ǥ�����
    char string[30] = {0,};//���ڿ� �����
    int count = 0, key = 0, len = 0, passlen = 10;//������ ����, �Է�Ű, ����, ����2(�н������)
    int temp = 0, i = 0, yes = 0;//���� 1, ���� 2
    STICK cntl = {NULL, NULL};//�ؽ�Ʈ(scanf)�� ���Ḯ��Ʈ
    STICK none = {NULL, NULL};//�ƹ��͵� �Է¾��Ҷ� NULL ������� ����.

    PERSON members[N];// ȸ�������� ����Ǵ� ����ü �迭


    //### 1. ���� �б� 	///////////////////////////////////////////////////////////
    FILE *fp;
    fp = fopen(DATAFILE, "rb");//�б�� ��������(read binary)�� ���������� ����


    //## 1-1 ����(������ ��������������)
    if (fp == NULL) {
        printf("��������������������������������������\n");
        printf("�������� ������ �������� �ʽ��ϴ�.  ��\n");
        printf("��                                  ��\n");
        printf("��-[%12s]������ �����մϴ�. ��\n", DATAFILE);
        printf("��������������������������������������\n\n");

        //������� �ʱⰪ �Է�
        filecntl.filecheck = FILECHECK;
        filecntl.delsize = 0;
        filecntl.size = 1;
        filecntl.error = 0;
        strcpy(filecntl.pw, "0000");
        system("PAUSE");
        fclose(fp);

        //���������� ����
        fp = fopen(DATAFILE, "wb");

        fwrite(&filecntl, sizeof(HEAD), 1, fp);

        for (i = 0; i < count; i++)
            fwrite(&members[i], sizeof(PERSON), 1, fp);

        fclose(fp);
    }

        //## 1-2 �ƴҰ�� (������ �����Ұ��)
    else {
        fread(&filecntl, sizeof(HEAD), 1, fp);
        //���Ͼպκ� (���������) �� �о�´�.

        //# 1-2-1 ���� ������ ����� ���� �̻��Ұ��
        if (filecntl.filecheck != FILECHECK || filecntl.error > 10000) {
            printf("������������������������������������������\n");
            printf("��[ERROR] ������ ������ �ջ�Ǿ����ϴ�. ��\n");
            printf("��                                      ��\n");
            printf("��     ���������ϸ�[ %-12s]      ��\n", DATAFILE);
            printf("������������������������������������������\n\n");
            fclose(fp);
            system("PAUSE");
            exit(-1);
        }

            //# 1-2-1 �׿� �����ϰ��
        else {
            //feof = ������ ��(End of file)�� ��� 1��, �ƴҰ�� 0����ȯ
            while (!feof(fp)) {
                //������ members[count]�� �а� count++
                fread(&members[count], sizeof(PERSON), 1, fp);
                count++;
            }
            //�� ������ count�� ������̴� --
            count--;
        }


    }
    system("cls");
    i = 0;
    //ȭ���� ����� ������ �ʱ�ȭ�մϴ�.


    //### 2. ��ȣ �Է�	///////////////////////////////////////////////////////////
    while (1) {
        Sleep(30);//�ݺ����� �ʹ� ������ ����Ǵ°� �����ϱ����ؼ�
        gotoxy(20, 6);
        printf("����������������������������������");
        gotoxy(20, 7);
        printf("��ȸ������ ���α׷� [verson 1.0]��");
        gotoxy(20, 8);
        printf("����������������������������������");
        gotoxy(25, 10);
        printf(" ��й�ȣ [ ");
        stpos = wherexy();
        printf("           ]");

        //textwindow�� �̳��� �̷��� �ָ� *�� ����մϴ�.
        string[0] = '*';
        if (filecntl.error == 0) {
            gotoxy(stpos.x - 8, stpos.y + 3);
            printf(">> [Tip] �ʱ��й�ȣ�� [ 0000 ]�Դϴ�.");
        }

        //������ scanf
        do { cntl = textwindow(stpos, string, &key, none, NONE, &passlen); } while (key != 13);
        sticktostr(cntl, string);

        if (strcmp(string, filecntl.pw) == 0) {//## 2-1���� ��ȣ�� �´ٸ�
            system("cls");
            if (filecntl.error > 3) {//��ȣ ����Ƚ���� 3���� �Ѵ´ٸ�
                printf("[Waring] %d���� ��й�ȣ �Է� �õ��� �־����ϴ�.", filecntl.error);
                printf("\n\n    - Ƚ���� �ʱ�ȭ�մϴ�.\n��������������������������������������\n\n");
                system("PAUSE");
            }
            filecntl.error = 0;//��ȣ ����Ƚ���� �ʱ�ȭ�մϴ�.
            break;//main����
        } else {//## 2-2 ��ȣ�� Ʋ�ȴٸ�
            gotoxy(stpos.x - 8, stpos.y + 3);
            filecntl.error++;
            i++;//��ȣ ����Ƚ���� ������ +1�մϴ�.(3���̻� Ʋ���� ���α׷�����)
            if (filecntl.error > 3 && i > 3) printf(">> [Error] ��й�ȣ ������ ����˴ϴ�.       \n\n");
            else printf(">> [Error] �߸��� ��й�ȣ�Դϴ�.                       \n\n", filecntl.error);

            if (filecntl.error > 3 && i > 3) {//## 2-2-1 3���̻� Ʋ�ȴٸ�

                //���������� ����
                fp = fopen(DATAFILE, "wb");

                fwrite(&filecntl, sizeof(HEAD), 1, fp);

                for (i = 0; i < count; i++)
                    fwrite(&members[i], sizeof(PERSON), 1, fp);
                fclose(fp);
                Sleep(1000);
                exit(-1);
            }
            //������ scanf�� ����� ���ڸ� ���������ϴ�.
            cntl.head = NULL;
            cntl.cur = NULL;
        }

    }
    //������ scanf�� ����� ���ڸ� ���������ϴ�.
    cntl.head = NULL;
    cntl.cur = NULL;

    //### 3. �޴� 	///////////////////////////////////////////////////////////
    while (key != 7) {
        //Sleep ���� �Է��� Ű���� ���� ����.
        //kbhit()�� Ű���� �Է°��� ������ 1, ������ 0�� ��ȯ
        while (kbhit()) getch();
        //getch() , ���۸� �����ʰ� Ű���忡�� �ٷ� �޾ƿ��� �Լ��� ���⿡ fflush()�� ������ �ʽ��ϴ�.


        system("cls");//ȭ�� �����
        gotoxy(0, 0);
        printf("��[ ȸ������ ]����������������������������������������������������������������\n");
        printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
        printf("��                                                                          ��\n");
        printf("�� [F1]���   [F2] ����   [F3] ����   [F4] ���      [F6] ����   [F7] ����  ��\n");
        printf("������������������������������������������������������������������������������\n");
        stpos = wherexy();

        //������ scanf
        textwindow(stpos, NULL, &key, cntl, NONE, &len);
        gotoxy(stpos.x, stpos.y);

        switch (key) {//F1~F7 �� ���� 1~7�� ��ȯ�մϴ�.
            case 1://��Ϻκ�
            {
                if (count < N)//�ִ밹���� �����ʾҴٸ�
                {
                    yes = add_members(members, count, 0);
                    //����� �����մϴ�.
                    if (yes == 1)//��ϵǾ��ٸ�
                    {
                        count++;
                        printf(" %-50s", "ȸ�������� ���������� ��ϵǾ����ϴ�.");
                        Sleep(300);
                    } else printf(" %-50s", "����� ����մϴ�.");//��ϵ��� ESC
                } else printf(" %-50s", "-�ִ��ο����� �ʰ��߽��ϴ�.");

                key = 0;//�Է�Ű�� �ʱ�ȭ�մϴ�.
                Sleep(500);
            }
                break;

            case 2: //�����κ�
            {
                if (count == 0) //������ 0�̶��
                {
                    printf(" -��ϵ� ȸ���� �����ϴ�.");
                    Sleep(400);
                    break;
                }

                temp = updata_members(members, count);
                //����(�޴�����)����
                if (temp == -3) {
                    //��ġ�ϴ� �ĺ���ȣ�� ���ٸ�
                    printf("\n\n-%-60s", "��ġ�ϴ� ���� �����ϴ�.");
                    Sleep(800);
                } else if (temp == 0) {
                    //���
                    printf(" %-50s", "������ ����մϴ�.");
                    Sleep(500);
                    break;
                } else {
                    //���(����)�մϴ�. temp�� �����ϰ���� ��ȣ.
                    yes = add_members(members, temp - 1, count);
                    if (yes == 1) {
                        printf(" %-50s", "ȸ�������� ���������� �����Ǿ����ϴ�.");
                        Sleep(300);
                    }
                        //�׿��߿� ESC�� �����ٸ�
                    else printf(" %-50s", "������ ����մϴ�.");
                    Sleep(500);
                }
            }
                break;

            case 3: {//ESC 0 , �����Ϸ� count��, �˻����� -3
                if (count == 0) {
                    printf(" -��ϵ� ȸ���� �����ϴ�.");
                    Sleep(400);
                    break;
                }

                temp = delete_members(members, count);
                //����(�޴�����)����
                if (temp == -3) {
                    printf("\n\n-%-60s", "��ġ�ϴ� ���� �����ϴ�.");
                    Sleep(300);
                } else if (temp == -1) printf("%-50s", "������ ����մϴ�.");
                else {

                    printf("%-52s", "������ �Ϸ�Ǿ����ϴ�.");
                    for (i = temp; i < count - 1; i++)
                        members[i] = members[i + 1];
                    count--;
                    Sleep(500);
                }
                Sleep(500);


            }
                break;

                //ȸ�����, ����Լ��ȿ� ���İ� �˻��� ���ԵǾ��ֽ��ϴ�.
            case 4:
                print_members(members, count);
                break;

            case 6: {    //������ ����, �������Լ� �ȿ� ��ȣ����� ��������� ���ԵǾ��ֽ��ϴ�.
                supervisor(members, count);

            }
                break;
            case 7: {
                //����� ȭ��
                gotoxy(0, 0);
                printf("��[ ȸ������ ]����������������������������������������������������������������\n");
                printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
                printf("��                                                                          ��\n");
                printf("�� [Enter] ��     [Anykey] �Ⱦ�                                             ��\n");
                printf("������������������������������������������������������������������������������\n");
                printf("��   ����   �� ���� �����Ͻðڽ��ϱ�?");
                printf("\n��������������\n");
                key = getch();
                //Enter�� �Է��ϸ� Key=7(while���ᰪ)����.
                if (key == 13) key = 7;
            }
                //�׿� ��Ÿ�Է��� ���� �����մϴ�.
            default :
                break;
        }


        //���������� ���� (�޴��� �ѹ� �����Ҷ�����)
        fp = fopen(DATAFILE, "wb");

        fwrite(&filecntl, sizeof(HEAD), 1, fp);

        for (i = 0; i < count; i++)
            fwrite(&members[i], sizeof(PERSON), 1, fp);
        fclose(fp);
    }
    //����Ǿ��� ��  ȭ��
    system("cls");
    printf("                          �� ���α׷��� ����˴ϴ�. ��\n\n ������ּż� �����մϴ�. <[-_- <]");
    Sleep(1000);
    return 0;
}

int supervisor(PERSON members[], int count) {
    POS mespos = {0, 0}, stpos = {0, 0}, repos = {0, 0};//���� ��ǥ�����
    STICK cntl = {NULL, NULL}, none = {NULL, NULL};//�ؽ�Ʈ (������ scanf)
    int key = 0, len = 0, i = 0, yes = -1, textlen = 10;//���� ������
    char string[30] = {0,};//���ڿ� ����

    while (key != 27) {
        Sleep(30);  //�� ���� ȭ��������� ���Ϸ��� ��� ������
        system("cls");// ȭ���� �����, �������߿� �Էµ� Ű�� �ٹ����ϴ�.
        while (kbhit()) getch();

        gotoxy(0, 0);
        printf("��[ ȸ������ ]����������������������������������������������������������������\n");
        printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
        printf("��                                                                          ��\n");
        printf("��   [F1] ȸ����� ���   [F2] ��ȣ����   [Esc] ����                        ��\n");
        printf("������������������������������������������������������������������������������\n");
        printf("�� �� �� �� ��");
        repos = wherexy();
        printf("\n��������������\n");
        stpos = wherexy();

        do { textwindow(mespos, NULL, &key, none, NONE, &len); } while (key != 1 && key != 2 && key != 27);

        switch (key) {
            case 1 ://ȸ��������
            {

                if (count == 0) {
                    gotoxy(repos.x, repos.y);
                    printf("��ϵ� ȸ���� �����ϴ�.");
                    Sleep(400);
                } else {
                    key = printfile(members, count);
                    if (key != 0) key = 27;
                }

            }
                break;

            case 2 : {//��й�ȣ ����
                gotoxy(stpos.x, stpos.y);
                printf(" ����������������������������������\n");
                printf(" ��������й�ȣ : ");
                mespos = wherexy();
                printf("               ��");
                printf("\n ����������������������������������\n");

                string[0] = '*';
                do { cntl = textwindow(mespos, string, &key, none, NONE, &textlen); } while (key != 27 && key != 13);
                sticktostr(cntl, string);

                if (key == 27) break;
                else if (strcmp(string, filecntl.pw) == 0) {
                    gotoxy(stpos.x, stpos.y);
                    printf(" ����������������������������������\n");
                    printf(" �������й�ȣ : ");
                    mespos = wherexy();
                    printf("               ��");
                    printf("\n ����������������������������������\n");

                    do { cntl = textwindow(mespos, NULL, &key, none, NONE, &textlen); } while (key != 27 && key != 13);
                    if (key == 27) break;
                    else {
                        sticktostr(cntl, string);
                        strcpy(filecntl.pw, string);
                        gotoxy(repos.x, repos.y);
                        printf("������ �Ϸ�Ǿ����ϴ�.");
                        Sleep(1000);
                        key = 27;
                    }
                } else {
                    gotoxy(repos.x, repos.y);
                    printf("[ERROR] �߸��� ��й�ȣ�Դϴ�.");
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

int printfile(PERSON members[], int count) {//ȸ����� �������
    POS stpos = {0, 0}, mespos = {0, 0};
    int key = 0, len = 0, i = 0, yes = 0;
    STICK cntl = {NULL, NULL};
    char string[20];
    FILE *fp = NULL;

    if (count == 0) return 0;

    gotoxy(0, 0);
    printf("��[ ȸ������ ]����������������������������������������������������������������\n");
    printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
    printf("��                                                                          ��\n");
    printf("��   [Esc] ����                                                             ��\n");
    printf("������������������������������������������������������������������������������\n");
    printf("�� ������� ��");
    stpos = wherexy();
    printf("\n��������������\n");
    printf(" ����������������������������\n");
    printf(" �����ϸ� : ");
    mespos = wherexy();
    printf("               ��");
    printf("\n ����������������������������");

    while (yes == 0 && key != 27) {
        do { cntl = textwindow(mespos, NULL, &key, cntl, FILENAME, &len); } while (key != 27 && key != 13);

        switch (key) {
            case 13 : {
                gotoxy(stpos.x, stpos.y);
                sticktostr(cntl, string);
                printf("[%s] ������ �����ϽǷ��� �ƹ�Ű�� ��������[��� ESC]", string);
                key = getch();

                if (key != 27) {
                    fp = fopen(string, "wt");
                    if (fp == NULL) {
                        gotoxy(stpos.x, stpos.y);
                        printf("&60s", "[ERROR] �߸��� ���ϸ��Դϴ�. �ٽ��Է����ּ���");
                        key = 0;
                    } else yes = 1;
                }

            }
                break;
            default :
                break;
        }
    }


    //������� ���� ���κ�(�Ӹ�����)
    fprintf(fp, PRINTHEAD);
    for (i = 0; i < count; i++)
        fprintf(fp, "[%d] %12s %12s %12s    %c�г�\n", i + 1, members[i].id, members[i].name, members[i].phone,
                members[i].grade);

    fclose(fp);

    if (yes == 0) {
        gotoxy(stpos.x, stpos.y);
        printf("%-64s", "��ҵ˴ϴ�.");
    } else {
        gotoxy(stpos.x, stpos.y);
        printf("%-64s", "����� �Ϸ�Ǿ����ϴ�.");
    }

    Sleep(500);
    return yes;
}


int delete_members(PERSON members[], int count) {//�����Լ�, �Լ������� �������ο� ������ �������������մϴ�.

//���������� ���κа� �����ϱ⿡(..) �����մϴ�.
    POS mespos = {0, 0}, stpos = {0, 0}, repos = {0, 0};
    STICK cntl = {NULL, NULL};
    int key = 0, len = 3, del = 0, oh = 0, i = 0, yes = -1;
    char string[30];

    while (key != 27 && key != 13) {

        gotoxy(0, 0);
        printf("��[ ȸ������ ]����������������������������������������������������������������\n");
        printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
        printf("��                                                                          ��\n");
        printf("��   [F1] ���   [Esc] ����                                                 ��\n");
        printf("������������������������������������������������������������������������������\n");
        printf("�� ȸ������ ��");
        repos = wherexy();
        printf("�ĺ���ȣ : ");
        mespos = wherexy();
        printf("\n��������������\n");
        stpos = wherexy();

        do { cntl = textwindow(mespos, NULL, &key, cntl, ID, &len); } while (key != 1 && key != 27 && key != 13);

        switch (key) {
            //ȸ�����
            case 1:
                print_members(members, count);
                break;
            case 13://���͸� �Է�������
            {
                //���� ���ڸ�
                sticktostr(cntl, string);
                del = atoi(string);//atoi( A to int)�� ���ڷ� ��ȯ�մϴ�.
                //stdlib ������Ͽ� �����մϴ�.

                //���ڸ� ������ ������ �ĺ���ȣ�� ���մϴ�.
                for (i = 0; i < count; i++)
                    if (del == members[i].count) {
                        yes = 1;//ã�Ҵٸ� 1
                        break; //Ż-��
                    }
                if (yes == -1) yes = -3;    //�׿� ��ã�Ҵٸ� -3
            }
                break;
                //�׿� ���Է��� ������ �������ݴϴ�.
            default :
                break;
        }
    }

    if (yes == 1) {    //ã�Ҵٸ�!
        gotoxy(repos.x, repos.y);
        printf("%-52s", " ������ �Ϸ� �ϽǷ��� �ƹ�Ű�� �������� [��� ESC]");
        gotoxy(stpos.x, stpos.y);
        printf(">> [%05d]%12s %12s %12s    %c�г� <<\n", members[i].count, members[i].id, members[i].name,
               members[i].phone, members[i].grade);
        printf(" ����������������������������������������������������������\n");

        key = getch();
        if (key == 27) yes = -1;
        else {
            yes = i;//ã�� count�� yes�����ְ�
            filecntl.delsize++;//������ ���������Ƿ� ���������� �߰��մϴ�.
            //�� ���� ���������� �������ٰ� �ð��������� ����Ǿ ������� �����Դϴ�.
        }
    }
    gotoxy(repos.x, repos.y);
    return yes; //yes�� ��ȯ�մϴ�.
}


int updata_members(PERSON members[], int count) {//�����Լ� �Լ������� �������ο� ������ ���� �ʿ��� ������ ��ȯ�մϴ�.
    //�ڷ����� ���� ����
    POS mespos = {0, 0}, stpos = {0, 0};
    STICK cntl = {NULL, NULL};
    int key = 0, len = 3, remake = 0, oh = 0, i = 0, yes = 0;
    char string[30];

    while (key != 27 && key != 13) {
        Sleep(30);
        while (kbhit()) getch();
        system("CLS");
        //�հ����� ���Ŀɴϴ�.. ��(�����Լ�)�� �����մϴ�!! ������ ����մϴ�(?)
        gotoxy(0, 0);
        printf("��[ ȸ������ ]����������������������������������������������������������������\n");
        printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
        printf("��                                                                          ��\n");
        printf("��   [F1] ���   [Esc] ����                                                 ��\n");
        printf("������������������������������������������������������������������������������\n");
        printf("�� ȸ������ ��");
        stpos = wherexy();
        printf("�ĺ���ȣ : ");
        mespos = wherexy();
        printf("\n��������������\n");

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


int sticktostr(STICK cntl, char *const string) {//�ؽ�Ʈ ���Ḯ��Ʈ(������ scanf)�� �������� ���ڿ��� �ٲ��ݴϴ�.
    int i = 0;
    for (i = 0; cntl.head != NULL; i++) {
        string[i] = cntl.head->data;
        cntl.head = cntl.head->next;
    }
    string[i] = '\0';

    return i;
}

int array_members(PERSON members[], int count) {//ȸ���� �����մϴ�.
    int i = 0, j = 0, data = 0;
    PERSON temp;
    POS mespos = {0, 0};

    gotoxy(0, 0);
    printf("��[ ȸ������ ]����������������������������������������������������������������\n");
    printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
    printf("��                                                                          ��\n");
    printf("��   [1] �й�   [2] �̸�    [Esc] ����                                      ��\n");
    printf("������������������������������������������������������������������������������\n");
    printf("�� ȸ������ ��");
    mespos = wherexy();
    printf("\n��������������\n");
    printf(" ��[00000]%12s %12s %12s   %6s��\n", "�й�", "�̸� ", "��ȭ��ȣ", "�г�");
    printf(" ����������������������������������������������������������\n");

    //F1�� ���޾� �Է��Ͽ� �׳� �Ѿ�� ��찡 ���Ƽ�
    //�����Ҷ��� F1 - ���� �Է����� �ٲ���ϴ�.
    while (data != 27 && data != '1' && data != '2') {
        data = getch();
        switch (data) {
            case '1': {//�й��� ��������.
                for (i = 1; i < count; i++)
                    for (j = 0; j < count - i; j++)
                        if (strcmp(members[j].id, members[j + 1].id) > 0) {
                            temp = members[j];
                            members[j] = members[j + 1];
                            members[j + 1] = temp;
                        }
                gotoxy(mespos.x, mespos.y);
                printf("�й� ������ ������ �Ϸ�Ǿ����ϴ�.");
                Sleep(500);
            }
                break;
            case '2': {//�̸��� ��������.
                for (i = 1; i < count; i++)
                    for (j = 0; j < count - i; j++)
                        if (strcmp(members[j].name, members[j + 1].name) > 0) {
                            temp = members[j];
                            members[j] = members[j + 1];
                            members[j + 1] = temp;
                        }
                gotoxy(mespos.x, mespos.y);
                printf("�̸� ������ ������ �Ϸ�Ǿ����ϴ�.");
                Sleep(500);
            }
                break;

                //�׿ܴ� �ų��� �������ݴϴ�.
            default :
                break;
        }
    }
    return 0;
}

int search_members(PERSON members[], int count) {//�˻��Լ�
    int i = 0, j = 0, data = 0, key = 0;
    int len[2] = {10, 12}, yes = 0;
    char string[20] = {0,};
    STICK cntl0 = {NULL, NULL}, cntl1 = {NULL, NULL};
    PERSON temp;
    POS mespos = {0, 0}, stpos = {0, 0}, nowpos = {0, 0};

    Sleep(30);
    system("cls");
    gotoxy(0, 0);
    printf("��[ ȸ������ ]����������������������������������������������������������������\n");
    printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
    printf("��                                                                          ��\n");
    printf("��   [1] �й�   [2] �̸�    [Esc] ����                                      ��\n");
    printf("������������������������������������������������������������������������������\n");
    printf("�� ȸ���˻� ��");
    mespos = wherexy();
    printf("\n��������������\n");
    printf(" ��[00000]%12s %12s %12s   %6s��\n", "�й�", "�̸� ", "��ȭ��ȣ", "�г�");
    printf(" ����������������������������������������������������������\n");
    stpos = wherexy();
    printf(" ����������������������������������������������������������");

    //�� ���� ����������
    //F1�� ���޾� �Է��ϴ°�찡 ���� ���ڷ�.
    while (data != 27 && data != '1' && data != '2') {
        data = getch();

        switch (data) {
            case '1': {
                gotoxy(mespos.x, mespos.y);
                printf("�Է� : ");
                nowpos = wherexy();
                do { cntl0 = textwindow(nowpos, NULL, &key, cntl0, ID, &len[0]); } while (key != 27 && key != 13);
                if (key == 27) break;//ESC �Է½� ����
                sticktostr(cntl0, string);
                strcpy(members[count].id, string);

                gotoxy(stpos.x, stpos.y);
                for (i = 0; i < count; i++)
                    if (strcmp(members[i].id, string) == 0) {
                        yes = 1;//ȸ���� ����ߴٸ� yes�� 1�� �־��ݴϴ�.
                        printf(" ��[%05d] %12s %12s %12s   %c�г⦢\n", members[i].count, members[i].id, members[i].name,
                               members[i].phone, members[i].grade);
                    }
                printf(" ����������������������������������������������������������");
            }
                break;

            case '2': {
                gotoxy(mespos.x, mespos.y);
                printf("�Է� : ");
                nowpos = wherexy();
                do { cntl1 = textwindow(nowpos, NULL, &key, cntl1, NONE, &len[1]); } while (key != 27 && key != 13);
                if (key == 27) break; //ESC �Է½� ����
                sticktostr(cntl1, string);
                strcpy(members[count].name, string);

                gotoxy(stpos.x, stpos.y);
                for (i = 0; i < count; i++)
                    if (strcmp(members[i].name, string) == 0) {
                        yes = 1;//ȸ���� ����ߴٸ� yes�� 1�� �־��ݴϴ�.
                        printf(" ��[%05d]%12s %12s %12s    %c�г⦢\n", members[i].count, members[i].id, members[i].name,
                               members[i].phone, members[i].grade);
                    }
                printf(" ����������������������������������������������������������");
            }
                break;

            default :
                break;
        }
    }
    gotoxy(mespos.x, mespos.y);
    if (key != 27 && data != 27) {

        if (yes == 1) {//ȸ���� ����ߴٸ�
            printf("�˻��� �Ϸ�Ǿ����ϴ�. [�ƹ�Ű] ����");
            getch();
        } else if (yes == 0) {//���ߴٸ�
            printf("�˻������ �������� �ʽ��ϴ�. [�ƹ�Ű] ����");
            getch();
        }
    }
    return 0;
}

int print_members(PERSON members[], int count) {//ȸ�� ����Լ�
    int i;
    POS mespos = {0, 0};
    POS stpos = {0, 0};
    int key = 0, len = 0;
    STICK cntl = {NULL, NULL};

    if (count > 0)//������ ȸ���� �ִ��� Ȯ���մϴ�.
    {
        //������ ���� ���Է��� �ٹ����ϴ�.
        while (kbhit()) getch();

        while (key != 27) {
            gotoxy(0, 0);
            printf("��[ ȸ������ ]����������������������������������������������������������������\n");
            printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
            printf("��                                                                          ��\n");
            printf("��   [F1] �˻�   [F2] ����   [Esc] ����                                     ��\n");
            printf("������������������������������������������������������������������������������\n");
            printf("�� ȸ����� ��");
            mespos = wherexy();
            printf("%52s", " ");
            printf("\n��������������\n");
            printf(" ��[00000]%12s %12s %12s   %6s��\n", "�й�", "�̸� ", "��ȭ��ȣ", "�г�");
            printf(" ����������������������������������������������������������\n");
            stpos = wherexy();
            printf(" ����������������������������������������������������������");

            gotoxy(stpos.x, stpos.y);
            for (i = 0; i < count; i++)
                printf(" ��[%05d]%12s %12s %12s    %c�г⦢\n", members[i].count, members[i].id, members[i].name,
                       members[i].phone, members[i].grade);
            printf(" ����������������������������������������������������������");

            do { textwindow(stpos, NULL, &key, cntl, NONE, &len); } while (key != 1 && key != 2 && key != 27);

            switch (key) {
                //�˻�
                case 1:
                    search_members(members, count);
                    break;
                    //����
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
        printf(" -��ϵ� ȸ���� �����ϴ�.");
        Sleep(400);
    }

    return 0;
}

int add_members(PERSON members[], int count, int remake) {//ȸ�� ����Լ�.(��������) �����ϰ� main�� �ƴ� �Լ� ������ ȸ�������͸� �ǵ帳�ϴ�.
//�����ÿ��� ��ü������ ������ ���ϴ� ��ȣ�� �ʿ��ϱ⿡ remake�� count�� �ٸ��Ծ��ϴ�.

    POS stpos = {0, 0}, mespos = {0, 0};
    int i = 0, key = 0, len[4] = {10, 12, 1, 1}, yes = 0, t = 0, ins;
    char string[30] = {0,};
    STICK cntl0 = {NULL, NULL}, cntl1 = {NULL, NULL}, cntl2 = {NULL, NULL}, cntl3 = {NULL, NULL};
    int check = 0;

    fflush(stdin);
    while (kbhit()) getch();

    gotoxy(0, 0);
    printf("��[ ȸ������ ]����������������������������������������������������������������\n");
    printf("��                                    ȸ���� [%3d] / �ִ� ȸ���� [%3d]      ��\n", count, N);
    printf("��                                                                          ��\n");
    printf("��  [Tap] �׸��̵�   [Esc] ���                                             ��\n");
    printf("������������������������������������������������������������������������������\n");
    if (remake >= 1)printf("�� ȸ������ ��");
    else printf("�� ȸ����� ��");
    mespos = wherexy();
    printf("\n��������������\n");

    if (remake >= 1) {
        printf(" ��[#%05d]��������������������������������������", members[count].count);
        stpos = wherexy(); //�����Ͻ�
        gotoxy(stpos.x - 2, stpos.y + 1);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 2);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 3);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 4);
        printf("��");
        gotoxy(stpos.x, stpos.y);
        printf(" \n ����  ��   : ");
        stpos = wherexy();
        printf("                ��[%12s]", members[count].id);
        printf("\n ����  ��   :                 ��[%12s]\n ����ȭ��ȣ :                 ��[%12s]\n ����  ��   :  �г�           ��[%7c �г�]",
               members[count].name, members[count].phone, members[count].grade);
        printf(" \n ������������������������������������������������\n");

    } else {
        printf(" ��[#%05d]��������������������", filecntl.size);
        stpos = wherexy(); //�����Ͻ�
        gotoxy(stpos.x - 2, stpos.y + 1);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 2);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 3);
        printf("��");
        gotoxy(stpos.x - 2, stpos.y + 4);
        printf("��");
        gotoxy(stpos.x, stpos.y);
        printf(" \n ����  ��   : ");
        stpos = wherexy();
        printf("\n ����  ��   : \n ����ȭ��ȣ :    -    -\n ����  ��   :  �г�");
        printf(" \n ������������������������������\n");
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
            //ID �Է°˻�
            if (cntl0.head == NULL) {
                check = 1;
                printf("%-50s", " [ERROR] �й�(����) �� �Է����ּ���.");
            }
                //�̸� �Է°˻�
            else if (cntl1.head == NULL) {
                check = 2;
                printf("%-50s", " [ERROR] �̸��� �Է����ּ���.");
            }
                //���� �Է°˻�
//			 else if(cntl2.head==NULL){check=3; printf("%-50s"," [ERROR] ��ȭ��ȣ(����) �� �Է����ּ���."); }
                //�г� �Է°˻�
            else if (cntl3.head == NULL) {
                check = 4;
                printf("%-50s", " [ERROR] �г�(1~4) �� �Է����ּ���.");
            }
                //�����Է�
            else {
                //ID �ߺ��˻�
                sticktostr(cntl0, string);
                if (remake == 0) {
                    for (i = 0; i < count; i++)
                        if (strcmp(members[i].id, string) == 0) {
                            check = 1;
                            printf("%-50s", " [ERROR] �ߺ��� �й��Դϴ�. �ٽ� �Է����ּ���.");
                            break;
                        }
                } else {
                    for (i = 0; i < remake; i++)
                        if (i != count && strcmp(members[i].id, string) == 0) {
                            check = 1;
                            printf("%-50s", " [ERROR] �ߺ��� �й��Դϴ�. �ٽ� �������ּ���.");
                            break;
                        }
                }

                //������ ���ٸ�
                if (check == 0) {
                    if (remake == 0) printf("%-52s", " ����� �Ϸ��ϽǷ��� �ƹ�Ű�� �������� [���Է� ESC]\n");
                    else printf("%-52s", " ������ �Ϸ��ϽǷ��� �ƹ�Ű�� �������� [���Է� ESC]\n");

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

        //����κ�

        sticktostr(cntl0, string);
        strcpy(members[count].id, string);
        sticktostr(cntl1, string);
        strcpy(members[count].name, string);
        sticktostr(cntl2, string);
        strcpy(members[count].phone, string);
        sticktostr(cntl3, string);
        members[count].grade = string[0];

        if (!(remake >= 1)) //������ �ƴҽÿ���
        {
            members[count].count = filecntl.size;
            (filecntl.size)++;
        }

    }
    gotoxy(mespos.x, mespos.y);

    return yes;

}
//////////////////////////////////////////////////////////////////////////////////////////
/////////////////�̹����δ� ���� ������scanf ���� �Լ����Դϴ�.////////////////////////////

//�ؽ�Ʈ          ���� ��ǥ       Ư�� �۾��Ҷ�   �Է�Ű    �ؽ�Ʈ    �޴���    ����*ins��
STICK textwindow(const POS stpos, char *string, int *key, STICK cntl, USE menu, int *len) {
    //getch()�� �ؽ�Ʈ�� �ϳ��ϳ� �Է¹޾� ���Ḯ��Ʈ�� ����ϴ�.
    //�Է��ϴٰ� Ư��Ű(Enter, F1 ���)�� �Է��ϸ� �Լ��� �ؽ�Ʈ�� ���� ���Ḯ��Ʈ�� ��ȯ�մϴ�.
    //�Է�Ű�� ���̰�, Ư�� �۾��Ҷ��� ���ڿ��� ��ȯ�ϱ����ؼ� �ּ�����(�����ͺ���)�� ����߽��ϴ�.

    STICK windo;//ȭ����¿� ���Ḯ��Ʈ(�ӽ������)
    int max = 0, cur = 0;//�ִ�Ŀ��, ����Ŀ��
    int data = 0, exdata = 0;//��, �߰���
    int ins = 1;//�μ�ƮŰ ���� (1, -1), �޴����� ���ؼ� ��ȯ
    int i = 0, j = 0, yes = 0;//���� ������

    *key = 0;//�Է�Ű �ʱ�ȭ(Ȥ�ó� �ؼ�)

    if (*len < 0)//�μ�Ʈ �б�
    {
        (*len) *= -1;
        ins = -1;

    }
    // �ִ���� �ʱ�ȭ
    if (menu == PHONE) *len = 11;
    else if (menu == GRADE) *len = 1;
    else if (menu == FILENAME) *len = 13;

    // �⺻�Է°�(FILENAME �ϰ��)
    if (menu == FILENAME && cntl.head == NULL) {
        cntl = addnode(cntl, '.');
        cntl = addnode(cntl, 't');
        cntl = addnode(cntl, 'x');
        cntl = addnode(cntl, 't');
        cntl.cur = NULL;
    }

    //�Է�Ű�� 0�϶� (Ư��Ű�� �ƴҶ�) �ݺ��մϴ�.
    while (*key == 0) {
        //�����ӵ�����
        Sleep(30);
        //Ŀ�� ����
        if (*len == 0) cursor_type(NO);
        else if (ins == 1) cursor_type(NORMAL);
        else cursor_type(SOLID);
        gotoxy(stpos.x, stpos.y);

        //ȭ����¿� ���Ḯ��Ʈ(�ӽ�����)
        windo.cur = cntl.cur;
        windo.head = cntl.head;

        //�޴��� ȭ�����
        switch (menu) {
            //�й�, �����̸�, �з������� �׳� ȭ�鿡 ���ڸ� �����ݴϴ�.
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
                //����(����� ����)
                gotoxy(stpos.x + i, stpos.y);
                putch(' ');
            }
                break;

                //������ ��쿡�� ���� "�г�" ���ڿ��� �ٿ��ݴϴ�.
            case GRADE: {
                gotoxy(stpos.x, stpos.y);
                if (windo.head != NULL) putch(windo.head->data);
                printf(" �г� ");
            }
                break;

                //��ȭ��ȣ�϶��� '-' �� �����ݴϴ�. (000-0000-0000)
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
                //����(����� ����)

                gotoxy(stpos.x + i, stpos.y);
                putch(' ');
                putch(' ');
                gotoxy(stpos.x + 3, stpos.y);
                putch('-');
                gotoxy(stpos.x + 8, stpos.y);
                putch('-');

            }
                break;

                //�׿� �߸��� �޴����� ���������� �����մϴ�.
            default:
                printf("textwindow- menu ERROR 01");
                exit(-1);
                system("CLS");
                break;
        }

        /////���Ḯ��Ʈ�� Ŀ���� ���������� �ٲߴϴ�.
        //(���ڼ�, cur= ������ġ, max= �ִ밪)
        cur = wherecur(cntl, CUR);
        max = wherecur(cntl, MAX);

        //��ȭ��ȣ�ϰ�쿡��, '-'�� �����ؼ� Ŀ���� �������ݴϴ�.
        if (menu == PHONE) {
            if (cur >= 3) cur = cur + 1;
            if (cur >= 8) cur = cur + 1;
        }

        //���������� �ٲ� Ŀ������ �̿��� Ŀ���� �����մϴ�.
        gotoxy(stpos.x + cur, stpos.y);



        //������ �Է� (����)
        data = getch();

        yes = 1;

        //�Է±��� ����(   ����		      Ư��2	      \b		  \r		cntl+x)
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

        ///�Է±��� �Ǵ�
        if (yes)
            switch (data) {
                default : {    //�ִ�ũ�� �ʰ�
                    if (*len == 1) {//1�����϶� �������� �˾Ƽ� ����� ���Է����ݴϴ�.
                        cntl = delnode(cntl, 1);
                        cntl = addnode(cntl, data);
                    } else if (max >= (*len)) {
                        //�μ�ƮŰ(����)�� ��������
                        //��ȭ��ȣ�϶��� �ִ���ڸ� 2���� �߰����ݴϴ�.('-' ����)
                        if (menu == PHONE && ins < 0 && (max + 2) > cur) {
                            cntl = delnode(cntl, -1);
                            cntl = addnode(cntl, data);
                        }
                            //ins�� �����ְ�, ������ Ŀ���� �ƴҰ��
                        else if (ins < 0 && max > cur) {
                            cntl = delnode(cntl, -1);
                            cntl = addnode(cntl, data);
                        }
                            //�׿�
                        else break;
                    }
                        //�����϶� ins�� �����������
                    else if (ins > 0) cntl = addnode(cntl, data);
                        //�����϶� ins�� �����������
                    else {
                        cntl = delnode(cntl, -1);
                        cntl = addnode(cntl, data);
                    }
                }
                    break;
                    //�Ϲ�Ű\b 8 / \r 13 / space 32 / Tap 9 / Esc 27 / cntl+ x 24
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

                    //Ư��Ű�� �ƽ�Ű�ڵ�� ǥ���Ұ���, (2����Ʈ) �̹Ƿ� 2���о��ݴϴ�.

                    //Ư��Ű(  0-**)F1 59 / F2 60 / F3 61 / F4 62 /... F10 68 /27 ESC
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
//							case 63 : *key=5; break; //F5�� �����ʽ��ϴ�.
                        case 64 :
                            *key = 6;
                            break;
                        case 65 :
                            *key = 7;
                            break;

                            //F8�̻��� �ٸ� Ư��Ű�� �ƽ�Ű �ڵ尡 ��ĥ�������Ƿ�
                            //F8 = 8 �� ǥ���� �Ұ����մϴ�.
                    }
                }
                    break;

                    //Ư��Ű( 224-**)��75 / ��77 / HOME 71 / END 79/ DEL 83 / INS 82 / F11 133 / F12 134
                case 224 : {
                    exdata = getch();
                    switch (exdata) {
                        case 71 :
                            cntl.cur = NULL;
                            break;
                            //Home �� �Է�������� Ŀ���� ���Ͼ����� �̵��մϴ�.

                        case 79 :
                            //End Ű�� �Է��������, ���� �ִ��� �Ǵ��ϰ� Ŀ���� ���� �ڰ����� �̵��մϴ�.
                            if (cntl.head == NULL) break;
                            else {
                                windo.head = cntl.head;

                                while (windo.head->next != NULL)
                                    windo.head = windo.head->next;

                                cntl.cur = windo.head;

                            }
                            break;

                            //Ű���� ����Ű�� ���������, ���ۺκ����� �Ǵ��ϰ� Ŀ���� �̵��մϴ�.
                        case 75 :
                            if (cntl.cur == NULL || cntl.head == NULL) break;
                            else cntl.cur = cntl.cur->prev;
                            break;
                            //Ű���� ������Ű�� ���������, ���κ����� �Ǵ��ϰ� Ŀ���� �̵��մϴ�.

                        case 77 :
                            if (cntl.head == NULL) break;//****�������� �̰Ŷ����� 3�ð��� ����
                            else if (cntl.cur == NULL) cntl.cur = cntl.head;
                            else if (cntl.cur->next == NULL)break;
                            else cntl.cur = cntl.cur->next;
                            break;

                            //insertŰ�� ���������, ins�� ��ȣ�� �ٲߴϴ�.
                        case 82 :
                            ins *= (-1);
                            break;
                            //Del�� ��������� �ݴ�������� ����ϴ�.
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

    if (*len != 0) (*len) *= ins; //�μ�Ʈ�� ��ȯ
    cursor_type(NO);//Ŀ������ : ����
    return cntl;//�ؽ�Ʈ ���Ḯ��Ʈ ��ȯ
}


STICK addnode(STICK cntl, char item) {
    NODE *temp;//��� ������ �����
    NODE *save;//�߰��� ���Ḯ��Ʈ

    //1. ù��° �Է�
    if (cntl.head == NULL) {
        cntl.head = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.head == NULL) {
            printf("ERROR add-01\n");
            exit(-1);
        }

        cntl.head->next = NULL;
        cntl.head->prev = NULL;
        cntl.head->data = item;

        cntl.cur = cntl.head; //Ŀ���̵�
    }

        //2. ù��° �Է��� �ƴѵ� Ŀ���� ���� ��
    else if (cntl.cur == NULL) {
        cntl.cur = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.cur == NULL) {
            printf("ERROR add-02\n");
            exit(-1);
        }

        cntl.cur->next = cntl.head;
        cntl.cur->prev = NULL;
        cntl.cur->data = item;

        cntl.head = cntl.cur;// ��带 ��ĭ ���� ������, ��������� Ŀ���� �̵�
    }

        //3. ������ �Է�
    else if (cntl.cur->next == NULL) {
        cntl.cur->next = (NODE *) calloc(1, sizeof(NODE));
        if (cntl.cur == NULL) {
            printf("ERROR add-03\n");
            exit(-1);
        }

        temp = cntl.cur;

        cntl.cur = cntl.cur->next; //Ŀ���̵�

        cntl.cur->next = NULL;
        cntl.cur->prev = temp;
        cntl.cur->data = item;

    }
        //4. �߰��Է�
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
        cntl.cur = cntl.cur->next;//Ŀ���̵�
    }

    return cntl;

}

STICK delnode(STICK cntl, int ins) {
    NODE *save;//�߰��Ұ�
    NODE *temp;//������(�ӽ�)

//ins  �������� , 0 ����Ŀ��(Ŀ������ ���ʱ���) , 1 ����Ŀ��(�����ʱ���)
    switch (ins) {
        case 1: {
            //1.���̾�����
            if (cntl.head == NULL) break;

                //2.���� �ִµ� Ŀ���� ���Ͼ��϶�
            else if (cntl.cur == NULL) break;

                //3.����������
            else if (cntl.cur->next == NULL) {
                temp = cntl.cur;

                //3.1 ���� �Ѱ��ۿ� ������
                if (cntl.head == cntl.cur) {
                    temp = cntl.cur;
                    free(temp);
                    cntl.head = NULL;
                    cntl.cur = NULL;
                }
                    //3.2 �׿�
                else {
                    //Ŀ���̵�
                    cntl.cur = cntl.cur->prev;
                    free(temp);
                    cntl.cur->next = NULL;
                }
            }

                //4.�߰�����
            else {
                temp = cntl.cur;
                save = cntl.cur->next;


                if (cntl.head == cntl.cur) {
                    cntl.head = save;
                    //Ŀ���̵�
                    cntl.cur = NULL;
                    save->prev = NULL;
                    free(temp);
                } else {    //Ŀ���̵�
                    cntl.cur = cntl.cur->prev;
                    free(temp);
                    cntl.cur->next = save;
                    save->prev = cntl.cur;
                }
            }
        }
            break;

        case -1: {
            //1.���̾�����
            if (cntl.head == NULL) break;

                //2.�����ִµ� Ŀ���� ���Ͼ��϶�
            else if (cntl.cur == NULL) {
                //2-1 ���� �ϳ��ۿ� ������
                if (cntl.head->next == NULL) {
                    temp = cntl.head;
                    free(temp);
                    cntl.head = NULL;
                }
                    //2-2 �׿�
                else {
                    temp = cntl.head;
                    save = cntl.head->next;

                    free(temp);
                    cntl.head = save;
                }
            }

                //3.����������
            else if (cntl.cur->next == NULL) break;

                //4.�߰�����
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

int wherecur(STICK cntl, INTCUR menu) {//Ŀ���� ����ִ��� ���������� ��ȯ���ݴϴ�.
    int i;
    switch (menu) {
        case CUR ://����Ŀ�� ã��
            if (cntl.head == NULL || cntl.cur == NULL) i = 0;
            else if (cntl.head == cntl.cur) i = 1;
            else {
                for (i = 1; cntl.head != cntl.cur; i++)
                    cntl.head = cntl.head->next;
            }
            break;

        case MAX ://�ִ����(������Ŀ��)ã��
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

// Ŀ���� ����ų� �ٽ� ǥ���Ѵ�.
void cursor_type(CURSOR_TYPE c) {
    CONSOLE_CURSOR_INFO CurInfo;
    //Ŀ���� ���¿� ���� ������ ���� ����ü, windows.h �� �����մϴ�.
    //dwsize ���ڿ� ä������ �ۼ�Ʈ , bvisible Ŀ�� on/off
    switch (c) {
        case NO://Ŀ���� 1%�� ä��� �������ʰԸ���ϴ�.
            CurInfo.dwSize = 1;
            CurInfo.bVisible = FALSE;
            break;
        case SOLID://Ŀ���� 100%ä��� ���̰Ը���ϴ�.
            CurInfo.dwSize = 100;
            CurInfo.bVisible = TRUE;
            break;
        case NORMAL://Ŀ���� 20% (_) �� ä��� ���̰Ը���ϴ�.
            CurInfo.dwSize = 20;
            CurInfo.bVisible = TRUE;
            break;
    }
    //�ܼ�â�� Ŀ�� ���¸� ���ϴ� �Լ�
    //�ܼ� ȭ���� �ڵ�(���� ��ũ����)�� ������ ����
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
}

void gotoxy(int x, int y) {//Ŀ���� �ش���ǥ�� �̵���ŵ�ϴ�.
    COORD Cur;
    //COORD ���� short�� x, y��ǥ�� ������, �� ��ǥ�� ȭ���� �ڵ��� �޾� ��ġ�� �ű�

    Cur.X = x;
    Cur.Y = y;
    //�ܼ�â�� Ŀ���� ���ϴ� ��ҷ� �ű涧 ���� �Լ�
    //���������� �ܼ�ȭ���� �ڵ�(���ϵ�ũ����)�� ������ �����մϴ�.
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

// Ŀ���� x,y ��ǥ�� �����Ѵ�.
POS wherexy() {
    POS pos = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO BufInfo;
    //��Ȯ�� �������� �������ִ����� ������
    //���� ȭ���� Ŀ�������� �������ִ°����� �뷫������ �˰��ֽ��ϴ�.

    //���������� �ܼ�ȭ���� �ڵ��������� Ŀ����ġ������ �Է��մϴ�.
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &BufInfo);
    //���� ��ǥ�� ��ȯ�մϴ�.
    pos.x = BufInfo.dwCursorPosition.X;
    pos.y = BufInfo.dwCursorPosition.Y;

    return pos;
}

