//必要なファイルをインクルード
#include <Mozzi.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>

//正弦波を使用
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin3(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin4(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin5(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, MOZZI_AUDIO_RATE> aSin6(SIN2048_DATA);

#define CONTROL_RATE 64

//変数の設定１：入力の読み取り
int value1; //A0(ボタン1,2)から読み取り→tone1,2へ
int value2; //A1(ボタン3,4)から読み取り→tone3,4へ
int value3; //A2(ボタン5,6)から読み取り→tone5,6へ
int value_chord; //A3(合竹つまみ)から読み取り→value_chord_modeへ
int value_distance; //A4(距離センサ)から読み取り→gainへ
int value_ratio; //A5(周波数つまみ)から読み取り→ratioへ
int value_pulse; //A6(パルスつまみ)から読み取り→velocityへ
bool stop; //D2(ストッパーボタン)から読み取り
bool play; //D4(active/muteボタン)から読み取り
bool pulse; //D7(パルスON/OFFボタン)から読み取り


//変数の設定２：読み取り値の処理
bool tone1; //ボタン1が押されたかどうか(押→false、離→true)
bool tone1_past; //tone1の一つ前の値
bool tone2; //ボタン2が押されたかどうか(tone1に同じ)
bool tone2_past;
bool tone3; //ボタン3が押されたかどうか(tone1に同じ)
bool tone3_past;
bool tone4; //ボタン4が押されたかどうか(tone1に同じ)
bool tone4_past;
bool tone5; //ボタン5が押されたかどうか(tone1に同じ)
bool tone5_past;
bool tone6; //ボタン6が押されたかどうか(tone1に同じ)
bool tone6_past;
int value_chord_mode; //合竹(和音)の種類
int value_distance_past; //value_distance_pastの一つ前の状態
byte gain; //出力のゲイン
float ratio; //基準周波数(A=430~860の範囲で変化)
int velocity; //パルスの周期(パルス：activeとmuteを交替する速さ)
byte gain_sub; //出力のゲイン、パルスがONの時に用いる
bool play_past; //playの一つ前の値
bool pulse_past; //pulseの一つ前の値


//変数の設定３：出力に関わる変数
bool tone1_status; //tone1の状態(発音→true、消音→false)
bool tone1_status_set; //tone1の状態(ストッパーボタンで音の変化を保留している時に用いる)
float tone1_freq; //tone1の周波数[Hz]
float tone1_freq_set; //tone1の周波数(ストッパーボタンで音の変化を保留している時に用いる)
bool tone2_status; //tone2の状態(以下tone1に同じ)
bool tone2_status_set;
float tone2_freq;
float tone2_freq_set;
bool tone3_status; //tone3の状態(以下tone1に同じ)
bool tone3_status_set;
float tone3_freq;
float tone3_freq_set;
bool tone4_status; //tone4の状態(以下tone1に同じ)
bool tone4_status_set;
float tone4_freq;
float tone4_freq_set;
bool tone5_status; //tone5の状態(以下tone1に同じ)
bool tone5_status_set;
float tone5_freq;
float tone5_freq_set;
bool tone6_status; //tone6の状態(以下tone1に同じ)
bool tone6_status_set;
float tone6_freq;
float tone6_freq_set;
bool play_status; //active(trure)かmute(false)か
bool pulse_status; //パルス機能がON(true)かOFF(false)か
bool pulse_on; //パルス機能がONの時、active(gain_sub=255:true)かmute(gain_sub=0:false)か
int fluc; //パルス機能用のタイマー(指定時間まで数える)
int8_t outData; //出力、アンプに送る電圧の値

//変数の設定４：周波数に関わる変数

/*
それぞれの竹管が持つ周波数(値が0の場合はmuteに設定する)
0:mute, 1:千(sen)F#6, 2:十(ju)G5, 3:下(ge)F#5, 4:乙(otsu)E5, 5:工(ku)C#5,
6:美(bi)G#5, 7:一(ichi)B4, 8:八(hachi)E6, 9:也(ya), 10:言(gon)C#6, 11:七(shichi)B5,
12:彳(gyo)A5, 13:上(jo)D6, 14:凢(bo)D5, 15:乞(kotsu)A4, 16:毛(mo), 17:比(hi)C6
也と毛については、音が出ない(使われない)竹管なので値を0とする
*/
float freqs[18] = 
{0, 1451.250, 764.444, 725.625, 645.000, 544.219, 816.328,
482.659, 1290.000, 0, 1088.438, 967.500, 860.000,
1146.667, 573.333, 430.000, 0, 1019.259};

/*
各合竹(和音のこと)の構成(値は、配列freqのindex)
七と彳は、全ての合竹に用いられる
0:mute
1:乞(kotsu):乞・八・千・乙・七・彳
2:一(ichi):凢・一・千・乙・七・彳
3:工(ku):凢・美・千・乙・七・彳
4:凢(bo):凢・八・千・乙・七・彳
5:乙(otsu):上・八・千・乙・七・彳
6:下(ge):上・美・千・下・七・彳
7:十(ju):上・八・十・下・七・彳
8:十[双調](ju[sojo]):上・八・十・七・彳
9:美(bi):上・美・千・比・七・彳
10:彳(gyo):上・八・千・七・彳
10比(hi):上・八・千・比・七・彳
*/
int tones[12][6] = {
  {0,0,0,0,0,0}, //mute0
  {15,8,1,4,11,12},  //kotsu1
  {14,7,1,4,11,12},  //ichi2
  {14,6,5,4,11,12},  //ku3
  {14,8,1,4,11,12},  //bo4
  {13,8,1,4,11,12},  //otsu5
  {13,6,1,3,11,12},  //ge6
  {13,8,2,3,11,12},  //ju7
  {13,8,2,0,11,12},  //ju(sojo)8
  {13,6,1,17,11,12},  //bi9
  {13,8,1,0,11,12},  //gyo10
  {13,8,1,17,11,12},  //hi11
};

//合竹の名称の配列(配列toneにおける合竹に対応した順番にしてある)
char aitake[12][4] = {"mut","kts","ich","ku","bo","ots","ge","ju","jus","bi","gyo","hi"};


//初期設定
void setup() {
  //Serial.begin(9600); //デバッグ用
  startMozzi(CONTROL_RATE);

  //ピンの設定
  pinMode(2,INPUT);
  pinMode(4,INPUT);
  pinMode(7,INPUT);
  pinMode(6, INPUT);

  //各変数の初期値設定
  tone1_past = true;
  tone2_past = true;
  tone3_past = true;
  tone4_past = true;
  tone5_past = true;
  tone6_past = true;
  play_past = false;
  pulse_past = false;
  pulse_on = true;
  tone1_freq = 0;
  tone2_freq = 0;
  tone3_freq = 0;
  tone4_freq = 0;
  tone5_freq = 0;
  tone6_freq = 0;
  tone1_status = false;
  tone2_status = false;
  tone3_status = false;
  tone4_status = false;
  tone5_status = false;
  tone6_status = false;
  play_status = false;
  pulse_status = false;
  value_distance_past = 0;
  gain_sub = 255;
  fluc = 0;
}

//繰り返し処理
void updateControl() {
  //入力値の読み取り
  value1 = mozziAnalogRead(0);
  value2 = mozziAnalogRead(1);
  value3 = mozziAnalogRead(2);
  value_chord = mozziAnalogRead(3);
  play = digitalRead(4);
  pulse = digitalRead(7);
  stop = !(digitalRead(2)); //ストッパーが作動している状態がfalse
  if(stop){ //ストッパーが作動していなければ
    value_distance = mozziAnalogRead(4);
    value_ratio = mozziAnalogRead(5);
    value_pulse = mozziAnalogRead(6);
  }

  //value1の値からtone1とtone2を判定
  if (value1 > 200 && value1 < 260){
    tone1 = false;
    tone2 = true;
  }else if (value1 > 320 && value1 < 380){
    tone1 = true;
    tone2 = false;
  }else if (value1 > 390 && value1 < 450){
    tone1 = false;
    tone2 = false;
  }else{
    tone1 = true;
    tone2 = true;
  }

  //value2の値からtone3とtone4を判定
  if (value2 > 200 && value2 < 260){
    tone3 = false;
    tone4 = true;
  }else if (value2 > 320 && value2 < 380){
    tone3 = true;
    tone4 = false;
  }else if (value2 > 390 && value2 < 450){
    tone3 = false;
    tone4 = false;
  }else{
    tone3 = true;
    tone4 = true;
  }

  //value3の値からtone5とtone6を判定
  if (value3 > 200 && value3 < 260){
    tone5 = false;
    tone6 = true;
  }else if (value3 > 320 && value3 < 380){
    tone5 = true;
    tone6 = false;
  }else if (value3 > 390 && value3 < 450){
    tone5 = false;
    tone6 = false;
  }else{
    tone5 = true;
    tone6 = true;
  }

  //value_chordの値から合竹の種類(value_chord_mode)の判定
  if(value_chord > 700){
    value_chord = 700;
  }
  value_chord_mode = value_chord / 59;

  //tone1_statusの判定
  if(tone1 && !(tone1_past)){ //tone1がfalse(ボタン押)→true(ボタン離)なら
    tone1_freq_set = freqs[tones[value_chord_mode][0]]; //周波数を変更し、発音(statusをtrueに)
    if (tone1_freq_set!=0){
      tone1_status_set = true;
    }
  }else if (!(tone1) && tone1_past){ //tone1がtrue(ボタン離)→false(ボタン押)なら
    tone1_freq_set = 0; //消音(statusをfalseに)
    tone1_status_set = false;
  }

  //tone2_statusの判定(tone1_statusの場合に同じ)
  if(tone2 && !(tone2_past)){
    tone2_freq_set = freqs[tones[value_chord_mode][1]];
    if (tone2_freq_set!=0){
      tone2_status_set = true;
    }
  }else if (!(tone2) && tone2_past){
    tone2_freq_set = 0;
    tone2_status_set = false;
  }

  //tone3_statusの判定(tone1_statusの場合に同じ)
  if(tone3 && !(tone3_past)){
    tone3_freq_set = freqs[tones[value_chord_mode][2]];
    if (tone3_freq_set!=0){
      tone3_status_set = true;
    }
  }else if (!(tone3) && tone3_past){
    tone3_freq_set = 0;
    tone3_status_set = false;
  }

  //tone4_statusの判定(tone1_statusの場合に同じ)
  if(tone4 && !(tone4_past)){
    tone4_freq_set = freqs[tones[value_chord_mode][3]];
    if (tone4_freq_set!=0){
      tone4_status_set = true;
    }
  }else if (!(tone4) && tone4_past){
    tone4_freq_set = 0;
    tone4_status_set = false;
  }

  //tone5_statusの判定(tone1_statusの場合に同じ)
  if(tone5 && !(tone5_past)){
    tone5_freq_set = freqs[tones[value_chord_mode][4]];
    if (tone5_freq_set!=0){
      tone5_status_set = true;
    }
  }else if (!(tone5) && tone5_past){
    tone5_freq_set = 0;
    tone5_status_set = false;
  }

  //tone6_statusの判定(tone1_statusの場合に同じ)
  if(tone6 && !(tone6_past)){
    tone6_freq_set = freqs[tones[value_chord_mode][5]];
    if (tone6_freq_set!=0){
      tone6_status_set = true;
    }
  }else if (!(tone6) && tone6_past){
    tone6_freq_set = 0;
    tone6_status_set = false;
  }

  //ストッパーが作動していなければ、周波数の変更を音に反映する
  if(stop){
    tone1_freq = tone1_freq_set;
    tone2_freq = tone2_freq_set;
    tone3_freq = tone3_freq_set;
    tone4_freq = tone4_freq_set;
    tone5_freq = tone5_freq_set;
    tone6_freq = tone6_freq_set;
    tone1_status = tone1_status_set;
    tone2_status = tone2_status_set;
    tone3_status = tone3_status_set;
    tone4_status = tone4_status_set;
    tone5_status = tone5_status_set;
    tone6_status = tone6_status_set;
  }

  //基準周波数の設定：A=430*ratio
  if(value_ratio > 700){
    value_ratio = 700;
  }
  ratio = (float)pow(2,value_ratio/700.00); //ratioは1~2の範囲を取る

  //ゲインの設定１：距離センサが急激に値を変えた(=センサから手を離した)場合、値を元のまま保持する
  if(value_distance < 40 && value_distance_past > 250){
    value_distance = value_distance_past;
  }

  //ゲインの設定２：value_distance(距離センサの値)からgainを算出する
  if(value_distance > 480){ //値の整形
    value_distance = 480;
  }else if(value_distance > 250 && value_distance < 350){
    value_distance = 350;
  }else if(value_distance < 250){
    value_distance = 480;
  }
  gain = map(value_distance,350,480,255,0); //gainの算出

  //パルスの設定１：パルスモードのON/OFF
  if(pulse && !(pulse_past)){ //パルスボタンが離→押された時、パルスモードのON/OFFを切り替える
    pulse_status = !(pulse_status);
  }

  //パルスの設定２：value_palseからvelocity(パルスのON/OFFの周期)を判定
  if(value_pulse > 700){ //値の整形
    value_pulse = 700;
  }
  velocity = map(value_pulse,0,700,1,33); //velocityの判定(1-33)

  //パルスの設定３：パルスの生成
  if(pulse_status || !(pulse_on)){ //パルスモードがONの時
    //flucを加算し、velocityと等しくなった時にON/OFFを切り替える
    fluc += 1;
    if(fluc >= velocity){
      if(gain_sub == 255){
        gain_sub = 0; //ON/OFFはgain_subの0/255で音声に反映させる
        pulse_on = false;
      }else if(gain_sub == 0){
        gain_sub = 255;
        pulse_on = true;
      }
      fluc = 0;
    }
  }

  //楽器全体のON/OFFの設定
  if(play && !(play_past)){ //active/muteボタンが押されたらactive/muteを切り替える
    play_status = !(play_status);
  }

  //音声への反映
  if(tone1_status){ //tone1_statusがtrueなら、aSin1の周波数を変更する 以下同様
    aSin1.setFreq(tone1_freq*ratio);
  }
  if(tone2_status){
    aSin2.setFreq(tone2_freq*ratio);
  }
  if(tone3_status){
    aSin3.setFreq(tone3_freq*ratio);
  }
  if(tone4_status){
    aSin4.setFreq(tone4_freq*ratio);
  }
  if(tone5_status){
    aSin5.setFreq(tone5_freq*ratio);
  }
  if(tone6_status){
    aSin6.setFreq(tone6_freq*ratio);
  }

  //デバッグ用
  /*
  Serial.print(value1);
  Serial.print(" ");
  Serial.print(value2);
  Serial.print(" ");
  Serial.print(value3);
  Serial.print("  ");
  Serial.print(value_chord);
  Serial.print(" ");
  Serial.print(value_ratio);
  Serial.print(" ");
  Serial.print(value_pulse);
  Serial.print("  ");
  Serial.print(value_distance);
  Serial.print("   ");
  Serial.print(stop);
  Serial.print(" ");
  Serial.print(pulse);
  Serial.print(" ");
  Serial.println(play);
  */
  /*
  Serial.print(tone1);
  Serial.print(" ");
  Serial.print(tone2);
  Serial.print(" ");
  Serial.print(tone3);
  Serial.print(" ");
  Serial.print(tone4);
  Serial.print(" ");
  Serial.print(tone5);
  Serial.print(" ");
  Serial.print(tone6);
  Serial.print("   ");
  Serial.print(stop);
  Serial.print(" ");
  Serial.print(pulse_status);
  Serial.print(" ");
  Serial.print(play_status);
  Serial.print("   ");
  Serial.print(gain);
  Serial.print(" ");
  Serial.print(aitake[value_chord_mode]);
  Serial.print(" ");
  Serial.print(ratio);
  Serial.print(" ");
  Serial.println(velocity);
  */
  /*
  Serial.print(tone1_freq);
  Serial.print(" ");
  Serial.print(tone2_freq);
  Serial.print(" ");
  Serial.print(tone3_freq);
  Serial.print(" ");
  Serial.print(tone4_freq);
  Serial.print(" ");
  Serial.print(tone5_freq);
  Serial.print(" ");
  Serial.print(tone6_freq);
  Serial.println(" ");
  */
  
  //値の受け継ぎ(現在の値を一つ前の値とする)
  tone1_past = tone1;
  tone2_past = tone2;
  tone3_past = tone3;
  tone4_past = tone4;
  tone5_past = tone5;
  tone6_past = tone6;
  play_past = play;
  pulse_past = pulse;
  value_distance_past = value_distance;
}

//再生用の関数
AudioOutput_t updateAudio(){
  outData = 0; //出力電圧
  if(play_status){ //active状態なら

    if(tone1_status){ //tone1のstatusがtrueなら
    outData += aSin1.next() / 8; //outData(出力)にaSin1(tone1)を反映　以下同様
    }
    if(tone2_status){
      outData += aSin2.next() / 8;
    }
    if(tone3_status){
      outData += aSin3.next() / 8;
    }
    if(tone4_status){
      outData += aSin4.next() / 8;
    }
    if(tone5_status){
      outData += aSin5.next() / 8;
    }
    if(tone6_status){
      outData += aSin6.next() / 8;
    }
    outData = outData * gain_sub >> 8; //パルスモードにおけるゲイン変化を反映
    outData = outData * gain >> 8;
  }
  
  return MonoOutput::from8Bit(outData); //出力
}

//再生用の関数
void loop(){
  audioHook();
}
