//=============================================================================
//
// バレット処理 [bullet.h]
// Author : 
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define BULLET_MAX		(5)			// バレットのMax数
#define BULLET_SPEED	(4.0f)		// バレットの移動スピード


// バレット構造体
struct BULLET
{
	BOOL					use;					// true:使っている  false:未使用
	float					w, h;					// 幅と高さ
	D3DXVECTOR3				pos;					// バレットの座標
	D3DXVECTOR3				rot;					// バレットの回転量
	D3DXVECTOR3				move;					// バレットの移動量
	int						countAnim;				// アニメーションカウント
	int						patternAnim;			// アニメーションパターンナンバー
	int						texNo;					// 何番目のテクスチャーを使用するのか

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

BULLET *GetBullet(void);
void SetBullet(D3DXVECTOR3 pos);


