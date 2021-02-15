#include <stdio.h>
#include <stdlib.h>
#define SIZE 997					//Tablo Büyüklüðü.
#define WORD_BUFFER 50				//Alýnan kelime için maks boyut.
#define HORNER_NUMBER 11			//Horner metodundaki kat sayý.

typedef struct node {
	char documentName[WORD_BUFFER]; //Doküman Adý
	struct node* next;				//Sonraki adres
}NODE;

typedef struct hash {
	char word[WORD_BUFFER];			//Kelime
	NODE* documentNameHead;			//Doküman adlarýný tutan linked listin baþý
}HASH;

typedef struct hashtable {
	HASH table[SIZE];				//Hash tablosu
	float loadfactor;				//doluYer/SIZE
	int indexCounter;				//doluYer
}HASHTABLE;

void prepareHashTable(HASHTABLE* hashTable);					//HashTablosunda alana tanýmlamalarý ve default deðerler ayarlanýr.
int hash1(long unsigned int key);								//1. hash fonksiyonu  h1(key) = key mod M	(index döner).
int hash2(long unsigned int key);								//2. hash fonksiyonu  h2(key) = 1 + (key mod MM)	(index döner).
void readFile(FILE* fp, HASHTABLE* hashTable, char filename[]);		//Kelimeleri okumak için.
int insertToHash(HASHTABLE* hashTable, long unsigned int key, char filename[WORD_BUFFER], char wrd[WORD_BUFFER]);		//Kelimelerei HashTablosuna almak için.	 (Tablonun dolup dolmadýðý döner.)
long unsigned int horner(char word[WORD_BUFFER]);					//Kelimelerin indexini hesaplarken sayýya dönüþümü için.	(kelimenin sayý hali döner).
void printHashTable(HASHTABLE* hashTable);							//Debug için hash tablosu yazdýrma.
void addDocumentName(NODE* head, char filename[WORD_BUFFER]);		//Hash tablosundaki kelimeye doküman adý ekleme.
void saveHashTable(HASHTABLE* hashTable);							//Hash tablosunu .txt olarak yazar.
HASHTABLE* readHashFile(FILE* hashFile);							//Dosyadan hash tablosu okuma.	(Hash tablosu döner.)
void findWord(HASHTABLE* hashTable, char wrd[WORD_BUFFER]);

int main() {

	int choice;		//Menü seçimleri için
	int quit = 0;	//Çýkýþ için
	char filename[WORD_BUFFER];		//Dosya adý
	HASHTABLE* hashTable;			//Hash tablosu
	FILE* fp;
	FILE * hashFile = fopen("17011033.txt", "r+");		//Hazýr hash tablosu var mý kontrolü.
	if (hashFile != NULL) {
    	printf("Hazir HashTable bulundu. Yukleniyor.\n");
        hashTable = readHashFile(hashFile);				//Hash tablosu okunur.
        fclose(hashFile);
    }
    else{
    	printf("Hashtable bulunamadi olusturuluyor.\n");
    	hashTable = (HASHTABLE*)malloc(sizeof(HASHTABLE));	//Hash tablosu oluþturuldu.
		prepareHashTable(hashTable);					
	}
	while(!quit){		
		printf("\nLoadfactor : %f\n1.Dokuman ekleme.\n2.Kelime Arama.\n3.Cikis.\n", hashTable->loadfactor);
		scanf("%d", &choice);
		system("CLS");
		if (choice == 1) {
			printf("Dokumanda kelimeler arasinda tek bosluk olduguna ve ozel karakterlerin bulunmadigina dikkat ediniz.\nDosya adi giriniz (input.txt) : ");
			scanf("%s", filename);
			fp = fopen(filename, "r");					//Doküman açýldý.
			if (fp == NULL) {
				printf("Dosya bulunamadi.");
				return 0;
			}
			readFile(fp, hashTable, filename);			//Döküman okundu ve hashe eklendi.
			fclose(fp);									//Döküman kapatýldý.
			saveHashTable(hashTable);					//Hash dosyasýna kaydedildi.
			printf("\nDokuman eklendi.\n");
			//printHashTable(hashTable);				//Debug için
		}
		else if (choice == 2) {
			char word[WORD_BUFFER];
			printf("Aranicak kelimeyi giriniz : ");
			scanf("%s", word);
			findWord(hashTable, word);					//Kelime arama
		}
		else {
			quit=1;										//Çýkýþ
		}
	}
	return 0;
}

void findWord(HASHTABLE* hashTable, char wrd[WORD_BUFFER]){
	int i = 0;
	int index;										//hash fonksiyonundan dönecek deðeri tutucak.
	long unsigned int key = horner(wrd); 					//Kelimenin sayý karþýlýðý alýndý.
	do {
		index = (hash1(key) + (i * hash2(key))) % SIZE;		//Hash fonksiyonlarýyla indexi bulundu.
		i++;
	} while (strcmp(hashTable->table[index].word, "-") && strcmp(hashTable->table[index].word, wrd) && i<SIZE);
	//Yukardaki döngü boþ bir yer veya ayný kelimeyi bulana kadar döner.

	if(!strcmp(hashTable->table[index].word, wrd)){			//Ayný kelime tabloda varsa.
		NODE* current = hashTable->table[index].documentNameHead;
		printf("Bulundu %s iceren dokumanlar : ", wrd);		//Dökümanlar yazdýrýlýr.
		while(current != NULL){
			printf(" %s ", current->documentName);
			current = current->next;
		}
	}
	else{
		printf("Bulunamadi.");
	}
	printf("\n");
}

HASHTABLE* readHashFile(FILE* hashFile){
	int i;
	char buffer[WORD_BUFFER];										//Dosyadan okunan kelimeleri tutan deðiþken.
	HASHTABLE* hashTable = (HASHTABLE*)malloc(sizeof(HASHTABLE));	//Hash tablosu için alan açýlýr.
	prepareHashTable(hashTable);									//Hash tablosu hazýrlanýr.
	fscanf(hashFile,"%f\n",&(hashTable->loadfactor));				//loadfactor okunur.
	fscanf(hashFile,"%d\n",&(hashTable->indexCounter));				//IndexCounter okunur.
	for(i=0;i<SIZE;i++){
		fscanf(hashFile, "%s", &buffer);							//Kelime okunur
		strcpy(hashTable->table[i].word, buffer);					//Tabloya alýnýr.
		fscanf(hashFile, "%s", &buffer);							//Boþluk geçilir.
		while(strcmp(buffer, ";")){									// ';' gelen kadar dosya isimlerini okur.
			addDocumentName(hashTable->table[i].documentNameHead, buffer);	// Dosya isimlerini ilgili kelimenin structýna ekler. 
			fscanf(hashFile, "%s", &buffer);
		}
	}
	return hashTable;
}
void saveHashTable( HASHTABLE* hashTable){
	int i;
	FILE* hashFile= fopen("17011033.txt", "w+");					//Dosya sýfýrlanýr / açýlýr.
	fprintf(hashFile,"%f\n",hashTable->loadfactor);					//Loadfactor yazýlýr.
	fprintf(hashFile,"%d\n",hashTable->indexCounter);				//Indexcounter yazýlýr.
	for(i=0;i<SIZE;i++){
		fprintf(hashFile,"%s ", hashTable->table[i].word);			//Kelime yazýlýr.
		NODE* current = hashTable->table[i].documentNameHead;		
		while(current!=NULL){										//Ýlgili dökümanlar yazýlýr.
			fprintf(hashFile,"%s ",current->documentName);
			current = current->next;
		}
		fprintf(hashFile,";\n");									//Satýr sonuna geldiðimizi belirten iþaret konur.
	}
}

void printHashTable(HASHTABLE* hashTable) {							//Debug için kullanýldý. Kontrol etmek isterseniz k
	int i,j;
	for (i = 0; i < SIZE; i++) {
		printf("%d - %s - ", i + 1, hashTable->table[i].word);
		NODE* current = hashTable->table[i].documentNameHead;
		while(current != NULL){
			printf(" %s - ", current->documentName);
			current = current->next;
		}
		printf("\n");
	}
	printf("\nLoadFactor : %f", hashTable->loadfactor);
	printf("\nTabloda dolu alan sayisi : %d\n", hashTable->indexCounter);
}

long unsigned int horner(char word[WORD_BUFFER]) {
	int i = 0;
	long unsigned int total = 0;								//Toplamý tutucak.
	long int multiple = 1;							//Her harfin çarpýlcaðý deðiþken.
	char letter;
	while (word[i] != NULL) {
		if (word[i] >= 65 && word[i] <= 90) {
			letter = word[i] - 'A';					//Büyük harfse 'A' çýkarýlýr.
		}
		else {
			letter = word[i] - 'a';					//Küçük harfse 'a' çýkarýlýr.
		}
													//En sonra oluþucak sayýyý küçültmüþ oldum.
		total = total + (letter * multiple);
		multiple *= HORNER_NUMBER;
		i++;
	}
	return total;
}

void addDocumentName(NODE* head, char filename[WORD_BUFFER]){
	NODE* current = head;													//Doküman listesinin baþýný tutar.
	while(current->next != NULL && strcmp(current->documentName, filename)){
		current = current->next;											//Doküman linked list tinin sonuna kadar ilerler
	}
	if(current->next == NULL && strcmp(current->documentName, filename)) {
		NODE* newNode = (NODE*)malloc(sizeof(NODE));
		strcpy(newNode->documentName, filename);							//Dökümaný ekler.
		newNode->next = NULL;
		current->next = newNode;
	}
	
}

int insertToHash(HASHTABLE* hashTable, long unsigned int key, char filename[WORD_BUFFER], char wrd[WORD_BUFFER]) {
	int i;
	int index;															//Hashten dönen index.
	if (hashTable->loadfactor < 1) {
		if (hashTable->loadfactor > 0.8) {
			printf("LOADFACTOR 0.8 den büyük. UYARI!!!!");				//LoadFactor kontrolü.
		}
		i = 0;
		do {
			index = (hash1(key) + (i * hash2(key))) % SIZE;				//Hash fonksiyonlarýndan dönen index.
			i++;
		} while (strcmp(hashTable->table[index].word, "-") && strcmp(hashTable->table[index].word, wrd) && i<SIZE);	//Boþ alan veya kelimenin aynýsýný bulunca çýkýcak.

		if (!strcmp(hashTable->table[index].word, "-")) {
			strcpy(hashTable->table[index].word, wrd);			//Boþ yer bulundu.
			(hashTable->indexCounter)++;						//Boþ yeri doldurduðumuz için artýrdýk.
			
		}
		addDocumentName(hashTable->table[index].documentNameHead, filename);	//Döküman adý eklenir.
		hashTable->loadfactor = (float) (hashTable->indexCounter) / SIZE;		//Loadfactor güncellenir.
	}
	else {
		printf("TABLO DOLU.");
		return 0;
	}
	return 1;
}

void readFile(FILE* fp, HASHTABLE* hashTable, char filename[WORD_BUFFER]) {
	char buffer[WORD_BUFFER];									//Kelimeyi tutucak deðiþken.
	int flag = 1;												//Loadfactorün kontrol ettiði flag.
	long unsigned int key;												//Kelimenin sayý karþýlýðý.
	while (flag && fscanf(fp, "%s", buffer) != EOF) {
		key = horner(buffer);									//Kelime sayýya çevrilir.
		flag = insertToHash(hashTable, key, filename, buffer);	//Hash tablosuna eklenir. Flag hash tablosunun dolup dolmadýðýný kontrol eder.
	}
	if (!flag) {
		printf("Tablo doldu. Taþan veriler : \n");
		while (flag && fscanf(fp, "%s", buffer) != EOF) {		//Taþan veriler yazdýrýlýr.
			printf("%s", buffer);
		}
	}
}

void prepareHashTable(HASHTABLE* hashTable) {
	int i;
	for (i = 0; i < SIZE; i++) {
		strcpy(hashTable->table[i].word,"-");								//Boþ alanlara -1 yazýlýr.
		hashTable->table[i].documentNameHead = (NODE*)malloc(sizeof(NODE));	//Doküman linked list için head tanýmlanýr.
		hashTable->table[i].documentNameHead->next = NULL;					//Next null yapýlýr.
		strcpy(hashTable->table[i].documentNameHead->documentName,"");		// Doküman adýna default deðer verilir.
	}
	hashTable->indexCounter = 0;											
	hashTable->loadfactor = 0;
}

int hash1(long unsigned int key) {
	return (key % SIZE);													//Birinci hash fonksiyonu.
}

int hash2(long unsigned int key) {
	return (1 + (key % (SIZE - 1)));										//Ýkinci hash fonksiyonu.
}
