📁 저장소 구조  
├─ Config/ # 프로젝트/엔진 설정(.ini)  
├─ Content/ChatX/ # 에셋(블루프린트, 위젯, 머티리얼 등)  
├─ Source/ # C++ 소스 ├─ ChatX.uproject # UE 프로젝트 파일  
└─ ChatX.sln.DotSettings.user # Rider 사용자 설정(개인 로컬)  
🏗️ 구현 단계  
모듈명/네임스페이스를 ChatX로 정리 기본  
GameMode/PlayerController/PlayerState 클래스 틀 추가  
AHWGameModeBase (게임 규칙/라운드/미니게임 관리)  
AHWPlayerController (입력/채팅 송신, HUD 인터페이스)  
AHWPlayerState (플레이어별 상태—예: 추측 횟수, 닉네임)  
  
2) 채팅 시스템  (기본)  
클라이언트 → 서버 → 모든 클라이언트로의 브로드캐스트  
클라에서 입력한 문자열을 Server RPC로 전달 서버가 메시지를 검증/가공 후 Client RPC로 전파  
```cpp  
ACXPlayerController::ServerSendChatMessage(FString InMsg)  
  
ACXGameModeBase::PrintChatMessageString(ACXPlayerController* Sender, const FString& Msg)  
```  
  
모든 ACXPlayerController에 ClientRPCPrintChatMessageString(CombinedMsg) 호출   

3) 숫자 맞추기 미니게임(Guess Game)  
서버는 SecretNumberString(3자리) 유지 클라가 채팅에 ...123처럼 입력 시 서버가 파싱  
유효성 체크(IsGuessNumberString) 후 카운트 증가 및 판정(JudgeResult)  
결과 형식: 사용자입력 -> ()S ()B 또는 정답 안내  
```cpp
const int32 Index = InChatMessageString.Len() - 3;
const FString GuessNumberString = InChatMessageString.RightChop(Index);
```

Client 입력 → ServerSendChatMessage → GameMode 판정 → 각 PlayerController.ClientRPCPrintChatMessageString(결과)  

4) 닉네임/접속 안내  
```cpp
ACXPlayerController->NotificationText = “Connected to the game server.”  
```
플레이어 이름: "Player" + AllPlayerControllers.Num()   
문자열 생성 시 (FString::FromInt) 사용, IsValid() 체크   

5) 네트워킹 정책  
RPC Client → Server: 입력/명령 전송  
Server → Client: 채팅/판정 결과 브로드캐스트  
Replication ACXPlayerState::CurrentGuessCount 등 진행 상황을 Replicated로 노출 가능  
DOREPLIFETIME 매크로로 동기화  

6) UI/HUD  
ChatLog : 스크롤 가능한 로그 영역 Input Line: Enter 제출 → 서버 RPC 호출  
System Notice: 연결/라운드 시작/정답 안내 에디터에서 Content/ChatX/ 아래 위젯/블루프린트 배치  

▶️ 빌드 & 실행 
.uproject 우클릭 → Generate Visual Studio project files Development Editor / Win64 빌드   
에디터 실행 후 Play In Editor (PIE) 멀티 테스트: New Editor Window (PIE) 다중 인스턴스 실행   
