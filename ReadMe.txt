LR2Twit
by kuna - http://kuna.wo.tc
Twitter - @kuna_KR
Released - 10.05.20

-----------------------------------------------------------------------------------
	☆ Release note
-----------------------------------------------------------------------------------
alpha ver - 5/18
 - first release
 - Autotwit Supported
 - options supported

alpha ver2 - 5/19
 - blocked duplicated running using mutex
 - and few fixes...

ver 1.0 - 5/20
 - few regex supported
 - more stabled
 - supports more exact infos and datas and etc...

ver 1.0b - 6/20
 - Log system added
 - Win7 x86/64, Win xp support.

ver 1.0c - 6/23
 - 発狂BMS difficulty supported.
 - you can set alarm sound (alarm.wav)
 - you can twit score by pressing "T" key (even in fullscreen!)
 - some Replacer(치환자) had changed.

ver 1.0d - 7/8
 - 発狂BMS list updated
 - 설정 추가 (T키 업로드)
 - 맥콤 표시 버그 수정
 - level.txt 인풋 엔진 수정
 - 프로그램 종료시 T키 바로가기 업로드 버그 수정

ver 1.0e alpha1 - 7/26 (커밋용)
 - DLL Injector 기본 클래스 구현
 - twitcurl, libcurl등의 모든 라이브러리들과 본체 프로그램을 vc2010으로 포팅 완료

ver 2.0 alpha1 - 8/3
 - DLL Injector 구현
 - 스크린샷 트윗 기능 구현
 - Destroy시 리소스가 제대로 해제되지 않던 일부 문제 개선
 - 자동트윗 취소, 툴팁, Language Pack, DirectX Overlay 알림 등의 편의성 기능 추가

-----------------------------------------------------------------------------------
	☆ 사용법
-----------------------------------------------------------------------------------
1. Twitter ID/PASS를 쓰고 Auth를 하여 토큰을 받습니다.
2. 옵션을 적절히 설정합니다. 끝 (...)

 - "T" 키로 스코어화면에서 언제든지 수동으로 트윗을 하실 수 있습니다 (1.0c)
 - LR2 윈도우를 찾았을 시, 트윗이 올라갈 시 알람 소리가 울립니다. 알람파일을 없애면 소리가 나지 않습니다 (1.0c)
 - settings.ini에서 세부적 설정이 가능하니 "팁" 란의 설명을 참조해 주세요.


-----------------------------------------------------------------------------------
	☆ 팁
-----------------------------------------------------------------------------------
1. 레겍스(치환자)를 이용하여 여러 언어, 혹은 다양한 방식으로 트윗 메시지를 설정할 수 있습니다.
settings.ini를 편집해 보세요.
ex)
TWIT_MESSAGE="[TITLE]" [SUBTITLE] 곡을 [GUAGE] 게이지로 [RESULT]하였습니다[AUTO]. EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2
TWIT_MESSAGE=I [RESULT]ed "[TITLE]" [SUBTITLE] song[AUTO]. EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2
TWIT_MESSAGE="[TITLE]" [SUBTITLE] 曲を　[GUAGE]のゲージで　[RESULT]しました[AUTO]。EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2

2. 타 국가권들을 위해서 다양한 인코딩을 지원하도록 신경 썼습니다.
기본 인코딩은 CP949(EUC-KR)이며, 일본의 경우에는 SHIFT_JIS로 옵션을 바꾸면 어플로케일 없이도 쉽게 프로그램 사용이 가능합니다.
ex)
TWIT_ENCODING=CP949
TWIT_ENCODING=SHIFT_JIS

3. 현재까지 지원하는 레겍스들은 다음과 같습니다.
[SCORE]
[PG]
[GR]
[GD]
[BD]
[PR]
[EXS] - EX Score
[EXHS] - EX High Score
[EXMS] - EX MAX Score
[NC] - Note count
[MC] - Max combo
[TITLE] - title of song (with [MAINTITLE] [SUBTITLE])
[MAINTITLE] - title of song - just title. (v1.0d)
[SUBTITLE] - subtitle (ex:7keys another / remixed by celas ....)
[ARTIST] - artist of song
[GENRE] - genre of song
[DIFF] - ☆00
[AUTO] - not writes anything but (AUTO-SCR) on setting auto scr
[IRTOT] - Internet Ranking total users
[IRNOW] - my max Internet Ranking

4. 건의사항은 @kuna_KR에 mention 해 주세요!



-----------------------------------------------------------------------------------
	☆ Future plan
-----------------------------------------------------------------------------------
 - supporting ruv-it!, Trilogy, Stepmania
 - custom judgement system for LR2
 - Multi/Alternative Twitter Account