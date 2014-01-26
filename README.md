LR2Twit
=======

Lunatic Rave 2의 스코어를 트위터에 올려서 자랑해 보세요!

by kuna - http://kuna.wo.tc

Twitter - @kuna_KR

Released - 10.05.20


* 가장 근래의 최신 버전은 https://github.com/kuna/LR2Twit/blob/master/redistribute/Redistribute.zip 을 다운받으시면 됩니다.


Release note
--------------
* alpha ver - 5/18
 - first release
 - Autotwit Supported
 - options supported

* alpha ver2 - 5/19
 - blocked duplicated running using mutex
 - and few fixes...

* ver 1.0 - 5/20
 - few regex supported
 - more stabled
 - supports more exact infos and datas and etc...

* ver 1.0b - 6/20
 - Log system added
 - Win7 x86/64, Win xp support.

* ver 1.0c - 6/23
 - 発狂BMS difficulty supported.
 - you can set alarm sound (alarm.wav)
 - you can twit score by pressing "T" key (even in fullscreen!)
 - some Replacer(치환자) had changed.

* ver 1.0d - 7/8
 - 発狂BMS list updated
 - 설정 추가 (T키 업로드)
 - 맥콤 표시 버그 수정
 - level.txt 인풋 엔진 수정
 - 프로그램 종료시 T키 바로가기 업로드 버그 수정

* ver 1.0e alpha1 - 7/26 (커밋용)
 - DLL Injector 기본 클래스 구현
 - twitcurl, libcurl등의 모든 라이브러리들과 본체 프로그램을 vc2010으로 포팅 완료

* ver 2.0 alpha1 - 8/3
 - DLL Injector 구현
 - 스크린샷 트윗 기능 구현
 - Destroy시 리소스가 제대로 해제되지 않던 일부 문제 개선
 - 자동트윗 취소, 툴팁, Language Pack, DirectX Overlay 알림 등의 편의성 기능 추가

* ver 2.0 alpha2 - 8/4
 - 잘못된 DLL 경로 수정
 - Fullscreen 에서의 Dx Overlay/Screenshot twit 기능 지원 처리 완료

* ver 2.0 alpha3 - 8/4
 - level.txt에 동일 제목명 노트로 구분하는 커맨드 추가
 - 레벨 구별시에 트림기능 추가 -> 발광난이도 레벨 인식률 상승
 - 보다 정확해진 게임 상태 인지 기능

* ver 2.0 alpha4 - 8/7
 - 발광레벨 인식 기능에 존재하던 치명적인 버그 픽스 및 인식률 향상 처리
 - level.txt에서 "key" 커맨드 사용 가능 -> PMS, 5Key, 14Key의 별도 난이도 지정 가능
 - level.txt DB에 DP/PMS 지원 추가
 - 몇가지 세팅 개선

* ver 2.0 alpha5 - ?
 - 글씨 깨짐 버그 수정
 - 트윗 리절트 화면 버그 인식 조율
 - 설정 부분의 인코딩 일부 수정 (TWIT_ENCODING=SHIFT_JIS 가 기본값으로 수정되었습니다)
 - level.txt DB 추가 수정

* ver 2.0 alpha6 - 9/22
 - iidx BMS의 경우에는 #LR2 해시태그가 붙지 않도록 변경되었습니다 (TWIT_MESSAGE의 서식이 약간 변경)
 -> 필터링을 끄시려면 iidx.txt를 지우시면 됩니다. (비권장)
 - 몇가지 사소한 오타 수정
 - 메모리 offset 일부 수정
 - DLL Inject 기능 개선

* ver 2.1 - 140127
 - UI 대폭 개선
 - twitcurl 라이브러리 다시 컴파일 및 적용으로 좀 더 확실해진 oAuth 인증
 - 스크린샷 업로드가 정상적으로 되지 않는 것으로 확인되었습니다.

사용법
----------------
1. Twitter ID/PASS를 쓰고 Auth를 하여 토큰을 받습니다.

2. 옵션을 적절히 설정합니다. 끝 (...)


* "T" 키로 스코어화면에서 언제든지 수동으로 트윗을 하실 수 있습니다 (1.0c)

* LR2 윈도우를 찾았을 시, 트윗이 올라갈 시 알람 소리가 울립니다. 알람파일을 없애면 소리가 나지 않습니다 (1.0c)

* settings.ini에서 세부적 설정이 가능하니 "팁" 란의 설명을 참조해 주세요.


팁
-----------
~ 1. 레겍스(치환자)를 이용하여 여러 언어, 혹은 다양한 방식으로 트윗 메시지를 설정할 수 있습니다.

settings.ini를 편집해 보세요.

ex)

TWIT_MESSAGE="[TITLE]" [SUBTITLE] 곡을 [GUAGE] 게이지로 [RESULT]하였습니다[AUTO]. EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2

TWIT_MESSAGE=I [RESULT]ed "[TITLE]" [SUBTITLE] song[AUTO]. EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2

TWIT_MESSAGE="[TITLE]" [SUBTITLE] 曲を　[GUAGE]のゲージで　[RESULT]しました[AUTO]。EX SCORE : [EXS]/[EXMS] - [RATE]%([RANK]) - [PG]/[GR]/[GD]/[BD]/[PR] #BMS #LR2

~ 2. 타 국가권들을 위해서 다양한 인코딩을 지원하도록 신경 썼습니다.

기본 인코딩은 CP949(EUC-KR)이며, 일본의 경우에는 SHIFT_JIS로 옵션을 바꾸면 어플로케일 없이도 쉽게 프로그램 사용이 가능합니다.

ex)

TWIT_ENCODING=CP949

TWIT_ENCODING=SHIFT_JIS

~ 3. 현재까지 지원하는 레겍스들은 다음과 같습니다.

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

~ 4. (LR2Twit ver 2.0 alpha 부터 적용되는 내용입니다)

 LR2Twit에서는 여러 단축키를 지원하는데, 이 기능을 사용하려면 옵션에서 "바로가기 키 활성화"를 체크해 주세요.

* Ctrl+A : DLL Re-Injection

* Ctrl+Y : 게임 도중 자동 트윗 취소

* Ctrl+T : 리절트 화면에서 텍스트 트윗

* Ctrl+Y : 리절트 화면에서 스크린샷 트윗 (딜레이 없음) - 주:자동 스크린샷 트윗의 딜레이는 3.5초 입니다.

~ 5. 전체화면에서 DX Overlay/Screenshot Twit 기능을 사용하려면, 먼저 "settings.ini"에서 "TWIT_DLLAUTOINJECTATSTARTUP" 값을 "0"으로 설정해 주신 후, LR2Twit 프로그램을 켠 후에 Lunatic Rave 2를 전체화면 상태에서 "Ctrl+A"로 수동 DLL Injection 시켜주셔야 합니다.

(alpha2) ... 프로그램 종특인지, 화면 모드가 수정되면 DLL Injection이 죽어버립니다 ㅡ,.ㅡ
