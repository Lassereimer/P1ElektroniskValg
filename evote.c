
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <ctype.h>

/* Constants */
#define TRUE 1
#define FALSE 0
#define CPR_LEN 10
#define DATA_LEN 16
#define EXIT_SUCCES 0

/* Prototypes */
int verifyIdentity(char *);
int isLeapYear(char *, char);
int BinarySearch(FILE **, char *, int *);
char *generateKey(void);
int getPower(double);
void RemoveCharacter(char *, char);

/* Main Function */
int main(void) {
  char c, CPR[CPR_LEN], person[2], party[2], key[DATA_LEN*2], data[DATA_LEN], data_hex[DATA_LEN*2], temp_string[2], temp_string2[2];
  int i, position = 0;
  unsigned int ch1, ch2;

  FILE *cp_temp = fopen("cpr_temp", "w+");
  FILE *cp = fopen("test", "r+");
  FILE *dp = fopen("data", "r+");

  srand(time(NULL));

  printf("Indtast venligst dit CPR-nummer: ");
  scanf("%[0-9]", CPR);

  if (!verifyIdentity(CPR)) {
    printf("Incorrect ID.\n");
  } else {

    if (BinarySearch(&cp, CPR, &position)) {
      printf("You have already voted and aren't allowed to vote again.\n");
    } else {
      fseek(cp, position, SEEK_SET);
      while ((c = getc(cp)) != EOF) {
        putc(c, cp_temp);
      }
      fseek(cp, position, SEEK_SET);
      fwrite(CPR, 1, sizeof(CPR), cp);
      fseek(cp, position + 12, SEEK_SET);
      fseek(cp_temp, 0, SEEK_SET);
      while ((c = getc(cp_temp)) != EOF) {
        putc(c, cp);
      }

      printf("Angiv venligst din stemme for parti og person: ");
      scanf(" %[A-Z]", party);
      scanf(" %[A-Z]", person);

      strncpy(data, CPR, CPR_LEN);
      strcat(data, ", ");
      strcat(data, party);
      strcat(data, ", ");
      strcat(data, person);

      strcpy(key, generateKey());

      printf("%s\n", key);

      for(i = 0; i < strlen(data); i++){
        sprintf((data_hex+i*2), "%02X", data[i]);
      }

      for (i = 0; i < DATA_LEN*2; i += 2) {
        strncpy(temp_string, data_hex + i, 2);
        strncpy(temp_string2, key + i, 2);
        sscanf(temp_string, "%02X", &ch1);
        sscanf(temp_string2, "%02X", &ch2);
        fprintf(dp, "%02X", ch1^ch2);
      }

    }
  }

  fclose(cp);
  return EXIT_SUCCES;
}

int verifyIdentity(char *CPR) {

  char CPR_Splittet[4][5];
  int i = 0;

  for (i = 0; i < CPR_LEN - 2 ; i += 2) {
    strncpy(CPR_Splittet[i/2], CPR + i, i == 6 ? 4 : 2);
    CPR_Splittet[i/2][i == 6 ? 4 : 2] = 0;
  }

  for (i = 0; i < 12; i++) {
    if (atoi(CPR_Splittet[1]) > 12 || atoi(CPR_Splittet[1]) < 1) {
      return FALSE;
    } else if (atoi(CPR_Splittet[1]) == 2) {
      if (atoi(CPR_Splittet[0]) > 28 + isLeapYear(CPR_Splittet[2], CPR_Splittet[3][0])) {
        return FALSE;
      }
    }
    else {
      if (atoi(CPR_Splittet[1]) == i) {
        if (atoi(CPR_Splittet[0]) > 30 + ((i <= 7) ? (i % 2) : ((i - 1) % 2))) {
          return FALSE;
        }
      }
    }
  }

  return TRUE;
}

int isLeapYear(char * yearLastDigits, char ciffer) {
  char yearString[4];
  int cifferInt;
  int yearLastDigitsInt;
  int year;

  yearLastDigitsInt = atoi(yearLastDigits);
  cifferInt = ciffer - '0';

  if (cifferInt >= 5 && cifferInt <= 8 && yearLastDigitsInt >= 58) {
    strcpy(yearString, "18");
  } else if (cifferInt <= 3 || ((cifferInt == 4 || cifferInt == 9) && yearLastDigitsInt >= 37)) {
    strcpy(yearString, "19");
  } else {
    strcpy(yearString, "20");
  }

  strcpy(yearString + 2, yearLastDigits);
  year = atoi(yearString);

  if ( (year % 4 == 0) && ( year % 100 != 0 || year % 400 == 0 ))
    return 1;
  else
    return 0;
}

int BinarySearch(FILE **cp, char *CPR, int *position) {

  int current_line, upper, lower = 0;
  double CPR_number, CPR_check;
  char CPR_check_string[CPR_LEN];

  sscanf(CPR, "%lf", &CPR_number);

  fseek(*cp, 0L, SEEK_END);
  upper = ftell(*cp)-12;

  while (lower <= upper) {
    current_line = (lower + (upper - lower) / 2) + (lower + (upper - lower) / 2) % 12;

    fseek(*cp, current_line, SEEK_SET);

    fscanf(*cp, "%[0-9]", CPR_check_string);

    sscanf(CPR_check_string, "%lf", &CPR_check);

    if (CPR_number == CPR_check) {
      *position = current_line;
      return TRUE;
    } else {
      if (CPR_number < CPR_check) {
        upper = current_line - 12;
      } else if (CPR_number > CPR_check) {
        lower = current_line + 12;
        if (lower >= upper) {
          current_line += 12;
        }
      }
    }
  }

  *position = current_line;

  return FALSE;
}

char *generateKey(void) {
  const char charset[] = "0123456789ABCDEF";
  char* key = malloc(DATA_LEN * 2 * sizeof(char));
  int i, charset_index;

  for (i = 0; i < DATA_LEN * 2; i++) {
    charset_index = rand() % (int) (sizeof charset - 1);
    if (!isalpha(charset[charset_index]) || (i + 1) % 2 == 0) {
      key[i] = charset[charset_index];
    } else {
      i--;
    }

  }
  key[DATA_LEN * 2] = '\0';

  return key;
}

char *encrypt(char key[DATA_LEN*2], char data[DATA_LEN]) {
  return "cake";
}

/* Maybe not necassary? */
void RemoveCharacter(char* source, char character)
{
  char* i = source;
  char* j = source;
  while(*j != 0)
  {
    *i = *j++;
    if(*i != character)
      i++;
  }
  *i = 0;
}

int getPower(double n) {
  int count = 0;

  while (n >= 2) {
    n /= 2;
    count++;
  }

  return count;
}
