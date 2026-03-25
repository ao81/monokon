#define USE_TIMER3_ISR
#include "mono_con.h"

/* 入力の定義 */
const int x_pin = A1;
const int D2 = 18; // SW
const int D3 = 19; // TSW
int x_pos, SW, TSW;
int prev_SW;
const int MIN_x = 0;
const int MAX_x = 1023;

/* 出力の定義 */
const int ledParam[2] = { B000, B100 };
const int hand7seg[3] = { 0x3f, 0x3e, 0x73 };

/* 手ごとの文字数 */
const int charCnt[3] = { 3, 6, 6 };

/* カウントダウンの時間 */
const int CD_TIME = 3000;

/* ゲームの状態の定義 */
typedef enum {
	INIT,
	WAIT,
	COUNTDOWN,
	JUDGE,
	STEP,
	FANFARE,
	GAMEOVER,
} GameStates;
GameStates gs;

/* じゃんけんの手の定義 */
typedef enum {
	GU,
	TYO,
	PA,
} Hand;
Hand pl_hand, cp_hand;

/* じゃんけんの結果の定義 */
typedef enum {
	AIKO,
	PLAYER,
	COMPUTER,
} Judge;
Judge judge;

/* ステップで使う変数 */
int stepCnt;

/* 割り込みで使う変数 */
word in_sw;
int cnt;

/**
 * @return プレイヤーの手
 */
Hand input_hand() {
	x_pos = analogRead(x_pin);
	if (x_pos > MAX_x - 300) return GU;
	else if (x_pos < MIN_x + 300) return PA;
	else return TYO;
}

/**
 * じゃんけんの結果判定
 * @param pl プレイヤーの手
 * @param cp コンピュータの手
 */
Judge janken_result(Hand pl, Hand cp) {
	return (Judge)(pl - cp + 3) % 3;
}

/**
 * ステッピングモータをnとjudgeに応じて回転させる
 * @param n ステップ数
 */
void step_spmotor(int n) {
	int dir = (judge == PLAYER) ? -1 : 1;

}

ISR(TIMER3_COMPA_vect) {
	if (in_sw > 5) {
		in_sw = 0;
		prev_SW = SW;
		SW = digitalRead(D2);
	}
	in_sw++;

	TSW = digitalRead(D3);

	if (cnt > -1) cnt--;

	// TODO: spmotorの処理
}

void setup() {
	config_init();
	serial_init();
	noTone(BZ_PIN);
	disp(num[10], num[10]);

	randomSeed(millis());

	PH = digitalRead(D1);
	SW = prev_SW = digitalRead(D2);
	TSW = digitalRead(D3);

	gs = INIT;
	pl_hand = GU;
	cp_hand = GU;
	judge = AIKO;
	stepCnt = 0;

	cnt = -1;
}

void loop() {
	if (TSW == LOW) {
		gs = INIT;
	} else {
		if (gs == INIT) gs = WAIT;
	}

	switch (gs) {
	case INIT:
		lm.color.GBR = ledParam[0];
		noTone(BZ_PIN);
		disp(num[10], num[10]);

		break;

	case WAIT:
		lm.color.GBR = ledParam[1];
		if (SW == LOW && prev_SW == HIGH) gs = COUNTDOWN;

		break;

	case COUNTDOWN:
		cnt = CD_TIME;
		if (cnt == 3000) disp(num[2], num[10]);
		if (cnt == 2000) disp(num[1], num[10]);
		if (cnt == 1000) disp(num[0], num[10]);
		if (cnt == 0) gs = JUDGE;

		break;

	case JUDGE:
		pl_hand = input_hand();
		cp_hand = (Hand)random(3);
		judge = janken_result(pl_hand, cp_hand);
		if (judge == AIKO) gs = WAIT;
		else gs = STEP;

		break;

	case STEP:
		

		break;

	case FANFARE:
		

		break;

	case GAMEOVER:
		

		break;

	}
}