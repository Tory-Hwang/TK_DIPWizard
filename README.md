# TK_DIPWizard
[개요]
- 영상 단말에서 네트워크로 차량 이미지를 수신 받아 차량 번호판을 분석하는 서버 프로그램으로 MVC 패턴으로 구현됨.
- 2010년 

- 프로그램명 : TK DIPWizard
  - https://blog.naver.com/tory0405/221335009527
  - https://blog.naver.com/tory0405/221335008538
  - https://blog.naver.com/tory0405/221335007490

[개발환경]
- Visual Studio 2013

[주요기능]
- TCP 서버
  - 영상 단말과의 네트워크 통신용 서버로 단말에서 촬영한 차량 이미지를 수신 받음.  
  - 최대 300개의 단말과 동시 통신 가능함. 
- 단말 제어
  - 영상 단말 제어    : 단말 리셋, 조명 제어
  
- 영상 분석 / 저장
  - 수신된 영상에서 차량의 위치, 번호판의 위치 , 번호판 문자 판별
  - 3층 구조의 Merchine Learning
  - 수신 영상의 저장 

[디렉토설명]
- TK_ALPRWizard   : MAIN Dialog, 영상 변환 GUI , 문자 학습 데이터 생성 파일  포함                    
- TK_DIP           : 이미지 프로세싱관련 DLL 소스 파일 포함
- TK_LPR          : 차량 번호판 적출 관련 DLL 소스 파일 포함
- TK_OCR          : 문자 판별 ML 알고리즘 DLL 소스 파일 
