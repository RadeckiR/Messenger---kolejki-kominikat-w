#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

struct msguser {
    long type;
    char log[1024];
    char pass[1024];
    int g;
    int sg[3];
    char mess[1024];
    int logged;
} user, user2;

struct msgcomunnicate {
    long type;
    int success;
    int value;
    char text[100];
} logowanie, zapytanie, odpowiedz;

struct msglist {
    long type;
    char positions[9][20];
    int ilosc;
} lista;

struct msgbuf {
    long type;
    char log[1024];
    char mess[1024];
    char to_user[1024];
    int to_group;
} message, odbior;


void login(int kolejka)
{
    while(1)
    {
        printf("Login:\n");
        scanf("%s", user.log);
        printf("Password:\n");
        scanf("%s", user.pass);
       
        user.type = 10;
        msgsnd(kolejka, &user, sizeof(user)-sizeof(long), 0);
        msgrcv(kolejka, &logowanie, sizeof(logowanie)-sizeof(long), 101, 0);
        if (logowanie.success == 1){
            printf("Logowanie poprawne!\n");
            break;
        } else { printf("blad logowania!\n"); }
            
    }
}

int main()
{
    int kolejka = msgget( 1000, 0644 );
    int dec, i,group;
    char c;
    printf("zaloguj sie:\n");
    login(kolejka);
    while (logowanie.success == 1)
    {
        printf("co chcesz zrobic:");
        printf("\n\t1. pokaz zalogowanych\n\t2. wyloguj sie");
        printf("\n\t3. pokaz uzytkownikow z danej grupy\n\t4. pokaz liste grup\n\t5. zapisz/wypisz sie z grupy");
        printf("\n\t6. wyslij wiadomosc do grupy\n\t7. wyslij wiadomosc do uzytkownika\n\t8. otworz skrzynke pocztowa\n");
        scanf("%d",&dec);
        switch(dec)
        {
            case 1:
                zapytanie.type = 200;
                msgsnd(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 0);
                msgrcv(kolejka, &lista, sizeof(lista)-sizeof(long), 223, 0);
                for(i=0; i<lista.ilosc; i++){
                    printf("%s\n", lista.positions[i]);
                }
                break;
            case 2:
                user.type = 300;
                msgsnd(kolejka, &user, sizeof(user)-sizeof(long),0 );
                msgrcv(kolejka, &logowanie, sizeof(logowanie)-sizeof(long), 101, 0);
                if (logowanie.success == 1) { printf("cos poszlo nie tak.\n"); }
                break;
            case 3:
                zapytanie.type = 400;
                printf("wybierz numer grupy 1-3\n");
                scanf("%d",&group);
                zapytanie.value = group-1;
                printf("%d", zapytanie.value);
                if(group<4 && group>0)
                {
                    msgsnd(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 0);
                    msgrcv(kolejka, &lista, sizeof(lista)-sizeof(long), 401, 0);
                    for(i=0; i<lista.ilosc; i++){
                        printf("%s\n", lista.positions[i]);
                    }
                }
                else
                {
                    printf("Bledny numer!\n");
                }
                break;
            case 4:
                zapytanie.type = 500;
                msgsnd(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 0);
                msgrcv(kolejka, &lista, sizeof(lista)-sizeof(long), 501, 0);
                for(i=0; i<3; i++){
                    printf("%s\n", lista.positions[i]);
                }
                break;
            case 5:
                user.type = 600;
           
                printf("Choose nr of group 1-3\n");
                scanf("%d", &group);
                user.g = group-1;
                printf("%d\n", group);
                printf("%d\n", user.g);
                if (group < 4 && group > 0){
                    msgsnd(kolejka, &user, sizeof(user)-sizeof(long), 0);
                    
                    msgrcv(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 601, 0);
                    if (odpowiedz.success == 1) { printf("uzytkownik zapisany do grupy nr %d\n", group); user.sg[user.g] = 1; }
                    else { printf("uzytkownik wypisany z grupy nr %d\n", group); user.sg[user.g] = 0; }
                }
                else{
                    printf("bledna liczba\n");
                }
                break;
            case 6:
                message.type = 700;
                strcpy(message.log, user.log);
                
                printf("wybierz nr grupy 1-3\n");
                scanf("%d", &group);
                message.to_group = group-1;
                if (group<4 && group>0)
                {
                    if (user.sg[message.to_group] == 1)
                    {
                        while((c = getchar()) != '\n' && c != EOF);
                        printf("podaj wiadomosc:\n");
                        fgets(message.mess, 1024, stdin);
                        printf("%s\n", message.mess);
                        msgsnd(kolejka, &message, sizeof(message)-sizeof(long), 0);
                        msgrcv(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 701, 0);
                        if (odpowiedz.success == 1) { printf("wiadomosc wyslana do grupy.\n"); }
                        else { printf("nie powiodlo sie - nie ma do kogo wyslac wiadomosci.\n"); }
                    }
                    else
                    {
                        printf("musisz najpierw zapisac sie do tej grupy.\n");
                    }
                }
                else
                {
                    printf("zly numer\n");
                }
                break;
            case 7:
                message.type = 800;
               
                strcpy(message.log, user.log);
                while((c = getchar()) != '\n' && c != EOF);
                printf("wpisz tresc wiadomosci\n");
                fgets(message.mess, 1024, stdin);
                printf("wpisz login odbiorcy:\n");
                scanf("%s", message.to_user);
                msgsnd(kolejka, &message, sizeof(message)-sizeof(long), 0);
                msgrcv(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 801, 0);
                if (odpowiedz.success == 1) { printf("wiadomosc wyslana do uzytkownika.\n"); }
                else { printf("uzytkownik nie istnieje.\n"); }
                break;
            case 8:
                zapytanie.type = 900;
               
                strcpy(zapytanie.text, user.log);
                msgsnd(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 0);
                msgrcv(kolejka, &odbior,sizeof(odbior)-sizeof(long),901,0);
                printf("twoje wiadomosci\n%s", odbior.mess);
                break;
            default:
                printf("bledny numer, wybierz poprawny\n");
                break;
        }
    }
    return 0;
}
