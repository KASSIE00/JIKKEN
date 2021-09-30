//=============================================================================
//
// �o���b�g���� [bullet.cpp]
// Author : 
//
//=============================================================================
#include "bullet.h"
#include "sprite.h"
#include "enemy.h"
#include "collision.h"
#include "score.h"
#include "sound.h"
#include "bg.h"

//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_WIDTH				(100/2)	// �L�����T�C�Y
#define TEXTURE_HEIGHT				(100/2)	// 
#define TEXTURE_MAX					(1)		// �e�N�X�`���̐�

#define TEXTURE_PATTERN_DIVIDE_X	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iX)
#define TEXTURE_PATTERN_DIVIDE_Y	(1)		// �A�j���p�^�[���̃e�N�X�`�����������iY)
#define ANIM_PATTERN_NUM			(TEXTURE_PATTERN_DIVIDE_X*TEXTURE_PATTERN_DIVIDE_Y)	// �A�j���[�V�����p�^�[����
#define ANIM_WAIT					(4)		// �A�j���[�V�����̐؂�ւ��Wait�l


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer				*g_VertexBuffer = NULL;				// ���_���
static ID3D11ShaderResourceView	*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����

static char *g_TexturName[] = {
	"data/TEXTURE/bullet00.png",
};

static BOOL	  g_Load = FALSE;		// ���������s�������̃t���O
static BULLET g_Bullet[BULLET_MAX];	// �o���b�g�\����


//=============================================================================
// ����������
//=============================================================================
HRESULT InitBullet(void)
{
	ID3D11Device *pDevice = GetDevice();

	//�e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		g_Texture[i] = NULL;
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TexturName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);


	// �o���b�g�\���̂̏�����
	for (int i = 0; i < BULLET_MAX; i++)
	{
		g_Bullet[i].use   = FALSE;			// ���g�p�i���˂���Ă��Ȃ��e�j
		g_Bullet[i].w     = TEXTURE_WIDTH;
		g_Bullet[i].h     = TEXTURE_HEIGHT;
		g_Bullet[i].pos   = D3DXVECTOR3(300, 300.0f, 0.0f);
		g_Bullet[i].rot   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Bullet[i].texNo = 0;

		g_Bullet[i].countAnim = 0;
		g_Bullet[i].patternAnim = 0;

		g_Bullet[i].move = D3DXVECTOR3(0.0f, -BULLET_SPEED, 0.0f);	// �ړ��ʂ�������
	}
	
	g_Load = TRUE;
	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitBullet(void)
{
	if (g_Load == FALSE) return;

	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}

	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i])
		{
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

	g_Load = FALSE;
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateBullet(void)
{
	BG *bg = GetBG();

	int bulletCount = 0;				// ���������o���b�g�̐�

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)	// ���̃o���b�g���g���Ă���H
		{								// Yes
			// �A�j���[�V����  
			g_Bullet[i].countAnim++;
			if ((g_Bullet[i].countAnim % ANIM_WAIT) == 0)
			{
				// �p�^�[���̐؂�ւ�
				g_Bullet[i].patternAnim = (g_Bullet[i].patternAnim + 1) % ANIM_PATTERN_NUM;
			}

			// �o���b�g�̈ړ�����
			g_Bullet[i].pos += g_Bullet[i].move;

			// ��ʊO�܂Ői�񂾁H
			if (g_Bullet[i].pos.y < bg->pos.y + (-g_Bullet[i].h/2))		// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}
			if (g_Bullet[i].pos.y > bg->pos.y + (SCREEN_HEIGHT+ g_Bullet[i].h/2))	// �����̑傫�����l�����ĉ�ʊO�����肵�Ă���
			{
				g_Bullet[i].use = false;
			}

			// �����蔻�菈��
			{
				ENEMY *enemy = GetEnemy();

				// �G�l�~�[�̐��������蔻����s��
				for (int j = 0; j < ENEMY_MAX; j++)
				{
					// �����Ă�G�l�~�[�Ɠ����蔻�������
					if (enemy[j].use == TRUE)
					{
						BOOL ans = CollisionBB(g_Bullet[i].pos, g_Bullet[i].w, g_Bullet[i].h,
							enemy[j].pos, enemy[j].w, enemy[j].h);
						// �������Ă���H
						if (ans == TRUE)
						{
							// �����������̏���
							enemy[j].use = FALSE;
							AddScore(1000);
							PlaySound(SOUND_LABEL_SE_bomb000);

							// �����̏��������Ȃ��Ɗђʒe�ɂȂ��Ă��܂�
							g_Bullet[i].use = FALSE;
							break;
						}
					}
				}
			}

			bulletCount++;
		}
	}


}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawBullet(void)
{
	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �}�g���N�X�ݒ�
	SetWorldViewProjection2D();

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// �}�e���A���ݒ�
	MATERIAL material;
	ZeroMemory(&material, sizeof(material));
	material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	BG *bg = GetBG();

	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == TRUE)		// ���̃o���b�g���g���Ă���H
		{									// Yes
			// �e�N�X�`���ݒ�
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_Bullet[i].texNo]);

			//�o���b�g�̈ʒu��e�N�X�`���[���W�𔽉f
			float px = g_Bullet[i].pos.x - bg->pos.x;	// �o���b�g�̕\���ʒuX
			float py = g_Bullet[i].pos.y - bg->pos.y;	// �o���b�g�̕\���ʒuY
			float pw = g_Bullet[i].w;		// �o���b�g�̕\����
			float ph = g_Bullet[i].h;		// �o���b�g�̕\������

			float tw = 1.0f / TEXTURE_PATTERN_DIVIDE_X;	// �e�N�X�`���̕�
			float th = 1.0f / TEXTURE_PATTERN_DIVIDE_Y;	// �e�N�X�`���̍���
			float tx = (float)(g_Bullet[i].patternAnim % TEXTURE_PATTERN_DIVIDE_X) * tw;	// �e�N�X�`���̍���X���W
			float ty = (float)(g_Bullet[i].patternAnim / TEXTURE_PATTERN_DIVIDE_X) * th;	// �e�N�X�`���̍���Y���W

			// �P���̃|���S���̒��_�ƃe�N�X�`�����W��ݒ�
			SetSpriteColorRotation(g_VertexBuffer, 
				px, py, pw, ph, 
				tx, ty, tw, th,
				D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f),
				g_Bullet[i].rot.z);

			// �|���S���`��
			GetDeviceContext()->Draw(4, 0);
		}
	}

}


//=============================================================================
// �o���b�g�\���̂̐擪�A�h���X���擾
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_Bullet[0];
}


//=============================================================================
// �o���b�g�̔��ːݒ�
//=============================================================================
void SetBullet(D3DXVECTOR3 pos)
{
	// �������g�p�̒e�����������甭�˂��Ȃ�( =����ȏ㌂�ĂȂ����Ď� )
	for (int i = 0; i < BULLET_MAX; i++)
	{
		if (g_Bullet[i].use == FALSE)		// ���g�p��Ԃ̃o���b�g��������
		{
			g_Bullet[i].use = TRUE;			// �g�p��Ԃ֕ύX����
			g_Bullet[i].pos = pos;			// ���W���Z�b�g
			PlaySound(SOUND_LABEL_SE_shot000);
			return;							// 1���Z�b�g�����̂ŏI������
		}
	}
}
