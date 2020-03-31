#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

struct msglist {
    long type;
    char positions[9][20];
    int ilosc;
} lista;

struct msguser {
    long type;
    char log[1024];
    char pass[1024];
    int g;
    int sg[3];
    char mess[1024];
    // char mess[10240];
    int logged;
} user, users[9], user2;

struct msggroup {
    char nazwa[1024];    //do odczytu z pliku, tylko GROUPS
    int subskrypcje[9];
    int licznosc_grupy;
    char mess[1024];
    //char lg[1024];    //lista grup
    //int sg[3];        //subskrypcja grupy
} groups[3];

struct msgcomunnicate {
    long type;
    int success;
    int value;
    char text[100];
} zapytanie, odpowiedz;

struct msgbuf {
    long type;
    char log[1024];
    char mess[1024];
    char to_user[1024];
    int to_group;
} message, delivered;

void logowanie()
{
	odpowiedz.success = 0;
	odpowiedz.type = 101;

	
	for (int i = 0; i < 9; i++) {
		if (strcmp(user.log, users[i].log) == 0 && strcmp(user.pass, users[i].pass) == 0) {
			odpowiedz.success = 1;

			users[i].logged = 1;
			printf("User: %s login\n", user.log);
			break;
		}

	}
}

void wczytywanie()
{
    FILE *file = fopen("dane.txt","r");
  
    int j, i;
   

    if (file == NULL){
        printf("plik uszkodzony\n");
    }
    else
    {
        for(i=0; i<9; i++){
            fscanf(file, "%s",users[i].log);
            printf("login %s\n", users[i].log);
            fscanf(file, "%s",users[i].pass);
        }
        for(j=0; j<3; j++){
            fscanf(file, "%s", groups[j].nazwa);
            printf("nazwa %s\n", groups[j].nazwa);
        }
    }
}



void pokaz_zalogowanych()
{
    int j = 0, i;
    lista.type = 201;
    for(i=0; i<9; i++)
    {
        if(users[i].logged == 1)
        {
            strcpy(lista.positions[j], users[i].log);
            j++;
        }
    }
    lista.ilosc = j;
}





void lista_grup()
{
    int i;
    lista.type = 501;
    for(i=0; i<3; i++)
    {
        strcpy(lista.positions[i], groups[i].nazwa);
    }
}

void subskrypcja_grupy(int g)
{
    odpowiedz.type = 601;
    for(int i=0; i<9; i++)
    {
        
        if (strcmp(user.log, users[i].log) == 0)
        {
            if (users[i].sg[g] == 0){
                users[i].sg[g] = 1;
                groups[g].subskrypcje[i] = 1;
                groups[g].licznosc_grupy++;
                odpowiedz.success = 1;
                printf("zapisano do grupy\n");
                break;
            } else {
                users[i].sg[g] = 0;
                groups[g].subskrypcje[i] = 0;
                groups[g].licznosc_grupy--;
                odpowiedz.success = 0;
                printf("wypisano z grupy\n");
                break;
            }
        }
    }
}
void wiadomosc_do_uzytkownika()
{
	int i;
	for (i = 0; i < 9; i++)
	{
		if (strcmp(users[i].log, message.to_user) == 0)
		{
			strcat(users[i].mess, message.log);
			strcat(users[i].mess, ": ");
			strcat(users[i].mess, message.mess);
			strcat(users[i].mess, "\n");
			odpowiedz.success = 1;
			break;
		}
		else { odpowiedz.success = 0; }
	}
	printf("User: %s to user: %s: %s\n", message.log, message.to_user, message.mess);
	odpowiedz.type = 801;
}

void wypisz_subskrypcje()
{
    for(int i=0; i<9; i++)
    {
        if (strcmp(user2.log, users[i].log) == 0){
            for(int j=0; j<3; j++)
            {
                if (users[i].sg[j] == 1){
                    printf("uzytkownik %s nalezy do grupy: %d", users[i].log, j);
                }
            }
        }
    }
   
}

void wylogowanie()
{
	odpowiedz.success = 1;
	odpowiedz.type = 101;
	for (int i = 0; i < 9; i++)
	{
		if (strcmp(user.log, users[i].log) == 0 && strcmp(user.pass, users[i].pass) == 0)
		{
			odpowiedz.success = 0;
			users[i].logged = 0;
			printf("User: %s logout\n", user.log);
			break;
		}

	}
}

void pokaz_z_danej_grupy(int g)
{
    int j=0, i;
    lista.type = 401;
    for(i=0; i<9; i++){
        if (users[i].sg[g] == 1){
            strcpy(lista.positions[j], users[i].log);
            j++;
        }
    }
    lista.ilosc = j;
}

void wiadomosc_do_grupy (int g)
{
    odpowiedz.success = 0;
    int i;
    for(i=0; i<9; i++){
        if (groups[g].subskrypcje[i] == 1) {
            strcat(users[i].mess, message.log);
            strcat(users[i].mess, " od ");
            strcat(users[i].mess, groups[g].nazwa);
            strcat(users[i].mess, ": ");
            strcat(users[i].mess, message.mess);
            strcat(users[i].mess, "\n");
        }
    }
    printf("User: %s pisz do grupy %s: %s\n", message.log, groups[g].nazwa, message.mess);
    odpowiedz.type = 701;
    odpowiedz.success = 1;
    
}


void odbior_wiadomosci()
{
    int i;
    for(i=0;i<9;i++)
    {
        if (strcmp(users[i].log, zapytanie.text) == 0)
        {
            strcpy(delivered.mess, users[i].mess);
            delivered.type = 901;
            strcpy(users[i].mess, "");
            break;
        }
    }
}


int main()
{
    int kolejka = msgget(1000,0644 | IPC_CREAT);
    wczytywanie();
  
    while(1)
    {
        
        if (msgrcv(kolejka, &user, sizeof(user)-sizeof(long), 10, IPC_NOWAIT) != -1)
        {
            printf("%s\n", user.pass);
            logowanie();
            msgsnd(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);
        }
        if(msgrcv(kolejka, &user, sizeof(user)-sizeof(long), 300, IPC_NOWAIT) != -1)
        {
            printf("%s\n", user.pass);
            wylogowanie();
            msgsnd(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 200, IPC_NOWAIT) != -1)
        {
            pokaz_zalogowanych();
            msgsnd(kolejka, &lista, sizeof(lista)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 400, IPC_NOWAIT) != -1)
        {
            pokaz_z_danej_grupy(zapytanie.value);
            msgsnd(kolejka, &lista, sizeof(lista)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 500, IPC_NOWAIT) != -1)
        {
            lista_grup();
            msgsnd(kolejka, &lista, sizeof(lista)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &user, sizeof(user)-sizeof(long), 600, IPC_NOWAIT) != -1)
        {
           
            subskrypcja_grupy(user.g);
            
            wypisz_subskrypcje();
            msgsnd(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &message, sizeof(message)-sizeof(long), 700, IPC_NOWAIT) != -1)
        {
            wiadomosc_do_grupy(message.to_group);
            msgsnd(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &message, sizeof(message)-sizeof(long), 800, IPC_NOWAIT) != -1)
        {
            wiadomosc_do_uzytkownika();
            msgsnd(kolejka, &odpowiedz, sizeof(odpowiedz)-sizeof(long), 0);
        }
        if (msgrcv(kolejka, &zapytanie, sizeof(zapytanie)-sizeof(long), 900, IPC_NOWAIT) != -1)
        {
            odbior_wiadomosci();
            msgsnd(kolejka,&delivered,sizeof(delivered)-sizeof(long),0);
        }
    }
    return 0;
    
}
