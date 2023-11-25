/* Scanner
 * @copyright (c) 2008, Hedspi, Hanoi University of Technology
 * @author Huu-Duc Nguyen
 * @version 1.0
 */

#include <stdio.h>
#include <stdlib.h>

#include "reader.h"
#include "charcode.h"
#include "token.h"
#include "error.h"
#include<ctype.h>

extern int lineNo;
extern int colNo;
extern int currentChar;

extern CharCode charCodes[];

/***************************************************************/

void skipBlank() {
  // TODO
  readChar();
}

void skipComment() {
  // (* và *) để đánh dấu điểm bắt đầu và kết thúc của chú thích
  // TODO
  int status = 0 ;// chưa gặp kết thúc của chú thích
  do{
    readChar();
    if(charCodes[currentChar] == CHAR_TIMES){
      readChar();
      if(charCodes[currentChar] == CHAR_RPAR){
        status = 1;
      }
    }
  }
  while(status == 0); // nếu bằng 0 : Chưa kết thúc comment 

  readChar();
}

Token* readIdentKeyword(void) {
  int index = 1;
  Token * identKey  = makeToken(TK_NONE,lineNo,colNo);
  identKey->string[0] = toupper(currentChar);
  readChar();
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_LETTER || charCodes[currentChar] == CHAR_DIGIT))
  {
    identKey->string[index++] = toupper(currentChar);
    readChar();
  }
  
  identKey->string[index]= "\0";
  identKey->tokenType = checkKeyword(identKey->string);
  if(identKey->tokenType  == TK_NONE){
    identKey->tokenType = TK_IDENT;
  }

  return identKey;
}

Token* readNumber(void) {
  // TODO

  int index = 1;
  Token * identKey  = makeToken(TK_NUMBER,lineNo,colNo);
  identKey->string[0] = currentChar;
  readChar();
  while (currentChar != EOF && (charCodes[currentChar] == CHAR_DIGIT))
  {
    identKey->string[index++] = currentChar;
    readChar();
  }
  
  identKey->string[index]= "\0";
  identKey->value  = atoi(identKey->string);

  return identKey;
}

Token* readConstChar(void) {
  // TODO

  readChar();
  Token* constChar = makeToken(TK_CHAR,lineNo,colNo);
  constChar->string[0] = currentChar;
  constChar->string[1] = "\0";
  readChar();
  if(charCodes[currentChar] == CHAR_SINGLEQUOTE){
    readChar();
    return constChar;
  }
}

Token* getToken(void) {
  Token *token;
  int ln, cn;

  if (currentChar == EOF) 
    return makeToken(TK_EOF, lineNo, colNo);

  switch (charCodes[currentChar]) {
  case CHAR_SPACE: skipBlank(); return getToken();
  case CHAR_LETTER: return readIdentKeyword();
  case CHAR_DIGIT: return readNumber();
  case CHAR_PLUS: 
    token = makeToken(SB_PLUS, lineNo, colNo);
    readChar(); 
    return token;
    // ....
    // TODO
    // case ( : có khả năng là comment
  case CHAR_LPAR :{
      readChar();
      switch (charCodes[currentChar])
      {
      case CHAR_TIMES :
        skipComment();
        return getToken();
        break;
      case  CHAR_PERIOD :
        token = makeToken(SB_LSEL,lineNo,colNo);
        return token;
        break;
      default:
        token = makeToken(SB_LPAR,lineNo,colNo);
        return token;
        break;
      }
  } 

  // dấu '
  case CHAR_SINGLEQUOTE:return readConstChar();
    // ....
    // dấu ;
  case CHAR_SEMICOLON:{
    Token* token =  makeToken(SB_SEMICOLON,lineNo,colNo);
    readChar();
    return token;
  }
  // dấu .
  case CHAR_PERIOD:{
    readChar();
    if(charCodes[currentChar] == CHAR_RPAR){
      token = makeToken(SB_RSEL,lineNo,colNo);
    }
    else {
     token =  makeToken(SB_PERIOD,lineNo,colNo-1);
    }
    return token;
  }
  // dấu :
  case CHAR_COLON:{
    readChar();
    if(charCodes[currentChar] == CHAR_EQ){
      token =  makeToken(SB_ASSIGN,lineNo,colNo);
    }
    else{
      token =  makeToken(SB_COLON,lineNo,colNo-1);
    }
    readChar();
    return token;
  }
  // dấu ,
  case CHAR_COMMA:{
        Token* token =  makeToken(SB_COMMA,lineNo,colNo);
        readChar();
        return token;
      } 
  // dấu =     
  case CHAR_EQ:{
    Token* token =  makeToken(SB_EQ,lineNo,colNo);
    readChar();
    return token;
  }
  // dấu <
  case CHAR_LT:{
    readChar();
    if(charCodes[currentChar] == CHAR_EQ){
    Token* token =  makeToken(SB_LE,lineNo,colNo);
    }
    else {
      Token* token =  makeToken(SB_LT,lineNo,colNo-1);
      }
    readChar();
    
    return token;
  }
  // >
  case CHAR_GT:{
    readChar();
    if(charCodes[currentChar] == CHAR_EQ){
    token =  makeToken(SB_GE,lineNo,colNo);
    }
    else {
      token =  makeToken(SB_GT,lineNo,colNo-1);
      }
    readChar();

    return token;
  }
  // -
  case CHAR_MINUS: 
    token = makeToken(SB_MINUS, lineNo, colNo);
    readChar(); 
    return token;
  case CHAR_TIMES:{
    token = makeToken(SB_TIMES, lineNo, colNo);
    readChar(); 
    return token;
  }
  // splash
  case CHAR_SLASH :{
    token = makeToken(SB_SLASH, lineNo, colNo);
    readChar(); 
    return token;
  }
  // 
  case CHAR_RPAR:{
    token = makeToken(SB_RPAR,lineNo,colNo);
    readChar();
    return token;
  }
  case CHAR_EXCLAIMATION:{
    readChar();
    if(charCodes[currentChar] == CHAR_EQ){
      token = makeToken(SB_NEQ,lineNo,colNo);
      readChar();
      return token;
    }
  }
  
  default:
    token = makeToken(TK_NONE, lineNo, colNo);
    error(ERR_INVALIDSYMBOL, lineNo, colNo);
    readChar(); 
    return token;
  }
}


/******************************************************************/

void printToken(Token *token) {

  printf("%d-%d:", token->lineNo, token->colNo);

  switch (token->tokenType) {
  case TK_NONE: printf("TK_NONE\n"); break;
  case TK_IDENT: printf("TK_IDENT(%s)\n", token->string); break;
  case TK_NUMBER: printf("TK_NUMBER(%s)\n", token->string); break;
  case TK_CHAR: printf("TK_CHAR(\'%s\')\n", token->string); break;
  case TK_EOF: printf("TK_EOF\n"); break;

  case KW_PROGRAM: printf("KW_PROGRAM\n"); break;
  case KW_CONST: printf("KW_CONST\n"); break;
  case KW_TYPE: printf("KW_TYPE\n"); break;
  case KW_VAR: printf("KW_VAR\n"); break;
  case KW_INTEGER: printf("KW_INTEGER\n"); break;
  case KW_CHAR: printf("KW_CHAR\n"); break;
  case KW_ARRAY: printf("KW_ARRAY\n"); break;
  case KW_OF: printf("KW_OF\n"); break;
  case KW_FUNCTION: printf("KW_FUNCTION\n"); break;
  case KW_PROCEDURE: printf("KW_PROCEDURE\n"); break;
  case KW_BEGIN: printf("KW_BEGIN\n"); break;
  case KW_END: printf("KW_END\n"); break;
  case KW_CALL: printf("KW_CALL\n"); break;
  case KW_IF: printf("KW_IF\n"); break;
  case KW_THEN: printf("KW_THEN\n"); break;
  case KW_ELSE: printf("KW_ELSE\n"); break;
  case KW_WHILE: printf("KW_WHILE\n"); break;
  case KW_DO: printf("KW_DO\n"); break;
  case KW_FOR: printf("KW_FOR\n"); break;
  case KW_TO: printf("KW_TO\n"); break;

  case SB_SEMICOLON: printf("SB_SEMICOLON\n"); break;
  case SB_COLON: printf("SB_COLON\n"); break;
  case SB_PERIOD: printf("SB_PERIOD\n"); break;
  case SB_COMMA: printf("SB_COMMA\n"); break;
  case SB_ASSIGN: printf("SB_ASSIGN\n"); break;
  case SB_EQ: printf("SB_EQ\n"); break;
  case SB_NEQ: printf("SB_NEQ\n"); break;
  case SB_LT: printf("SB_LT\n"); break;
  case SB_LE: printf("SB_LE\n"); break;
  case SB_GT: printf("SB_GT\n"); break;
  case SB_GE: printf("SB_GE\n"); break;
  case SB_PLUS: printf("SB_PLUS\n"); break;
  case SB_MINUS: printf("SB_MINUS\n"); break;
  case SB_TIMES: printf("SB_TIMES\n"); break;
  case SB_SLASH: printf("SB_SLASH\n"); break;
  case SB_LPAR: printf("SB_LPAR\n"); break;
  case SB_RPAR: printf("SB_RPAR\n"); break;
  case SB_LSEL: printf("SB_LSEL\n"); break;
  case SB_RSEL: printf("SB_RSEL\n"); break;
  }
}

int scan(char *fileName) {
  Token *token;

  if (openInputStream(fileName) == IO_ERROR)
    return IO_ERROR;

  token = getToken();
  while (token->tokenType != TK_EOF) {
    printToken(token);
    free(token);
    token = getToken();
  }

  free(token);
  closeInputStream();
  return IO_SUCCESS;
}

/******************************************************************/

int main(int argc, char *argv[]) {
  if (argc <= 1) {
    printf("scanner: no input file.\n");
    return -1;
  }

  if (scan(argv[1]) == IO_ERROR) {
    printf("Can\'t read input file!\n");
    return -1;
  }
    
  return 0;
}



