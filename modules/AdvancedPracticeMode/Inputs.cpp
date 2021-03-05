//
// Created by PinkySmile on 03/03/2021.
//

#include <SokuLib.hpp>
#include <list>
#include <utility>
#include "DrawUtils.hpp"
#include "Inputs.hpp"
#include "Gui.hpp"
#include "State.hpp"

namespace Practice
{
#define BOX_WIDTH 140
#define BOX_HEIGHT 340
#define MIN_ALPHA 0.5
#define MAX_ALPHA 1.0
#define MIN_BOX_ALPHA 0.1
#define MAX_BOX_ALPHA 0.4
#define SPRITE_SIZE 24

#define A_SPRITE_POS         Vector2<int>{0,   28}
#define B_SPRITE_POS         Vector2<int>{32,  28}
#define C_SPRITE_POS         Vector2<int>{64,  28}
#define D_SPRITE_POS         Vector2<int>{96,  28}
#define CH_SPRITE_POS        Vector2<int>{128, 28}
#define SC_SPRITE_POS        Vector2<int>{160, 28}
#define UP_SPRITE_POS        Vector2<int>{0,   -4}
#define DOWN_SPRITE_POS      Vector2<int>{32,  -4}
#define LEFT_SPRITE_POS      Vector2<int>{64,  -4}
#define RIGHT_SPRITE_POS     Vector2<int>{96,  -4}
#define LEFTUP_SPRITE_POS    Vector2<int>{128, -4}
#define RIGHTUP_SPRITE_POS   Vector2<int>{160, -4}
#define RIGHTDOWN_SPRITE_POS Vector2<int>{192, -4}
#define LEFTDOWN_SPRITE_POS  Vector2<int>{224, -4}
#define BE_SPRITE_POS        Vector2<int>{192, 28}
#define HJ_SPRITE_POS        Vector2<int>{224, 32}
#define DASH_SPRITE_POS      Vector2<int>{256, 32}
#define AIR_SPRITE_POS       Vector2<int>{256, 0}
#define FAR_SPRITE_POS       Vector2<int>{288, 32}

	struct MoveState {
		SokuLib::Action action;
		unsigned frames;
		unsigned short animation;
		unsigned short subFrames;
	};

	struct Input {
		SokuLib::KeyInput input;
		unsigned duration;
		SokuLib::Action action;
	};

	struct MoveSpriteDescriptor {
	private:
		bool _isSkill = false;
		std::vector<std::string> _sequences;
		Sprite &_sprite;
		Sprite &_sprite2;
		Vector2<int> _startRect;
		std::vector<Vector2<int>> _rects;
		Vector2<unsigned> _rectSize;

		void _drawSkill(Vector2<int> pos, Vector2<unsigned> size, bool reverse, SokuLib::Character character) const
		{
			std::vector<Vector2<int>> rects;

			for (auto c : this->_sequences[character]) {
				switch (c) {
				case '1':
					rects.push_back(LEFTDOWN_SPRITE_POS);
					break;
				case '2':
					rects.push_back(DOWN_SPRITE_POS);
					break;
				case '3':
					rects.push_back(RIGHTDOWN_SPRITE_POS);
					break;
				case '4':
					rects.push_back(LEFT_SPRITE_POS);
					break;
				case '6':
					rects.push_back(RIGHT_SPRITE_POS);
					break;
				case 'b':
					rects.push_back(B_SPRITE_POS);
					break;
				case 'c':
					rects.push_back(C_SPRITE_POS);
					break;
				default:
					abort();
				}
			}
			this->_sprite.setSize({max(size.x, size.y), max(size.x, size.y)});
			if (reverse) {
				for (unsigned i = rects.size(); i > 0; i--) {
					this->_sprite.setPosition(pos);
					this->_sprite.rect.top = rects[i - 1].y;
					this->_sprite.rect.left = rects[i - 1].x;
					this->_sprite.rect.width = this->_rectSize.x;
					this->_sprite.rect.height = this->_rectSize.y;
					this->_sprite.draw();
					pos.x -= size.x;
				}
				this->_sprite2.setPosition(pos);
				this->_sprite2.rect.top = this->_startRect.y;
				this->_sprite2.rect.left = this->_startRect.x;
				this->_sprite2.rect.width = this->_rectSize.x;
				this->_sprite2.rect.height = this->_rectSize.y;
				this->_sprite2.draw();
			} else {
				this->_sprite2.setPosition(pos);
				this->_sprite2.rect.top = this->_startRect.y;
				this->_sprite2.rect.left = this->_startRect.x;
				this->_sprite2.rect.width = this->_rectSize.x;
				this->_sprite2.rect.height = this->_rectSize.y;
				this->_sprite2.draw();
				for (const auto &rect : rects) {
					pos.x += size.x;
					this->_sprite.setPosition(pos);
					this->_sprite.rect.top = rect.y;
					this->_sprite.rect.left = rect.x;
					this->_sprite.rect.width = this->_rectSize.x;
					this->_sprite.rect.height = this->_rectSize.y;
					this->_sprite.draw();
				}
			}
		}

	public:
		MoveSpriteDescriptor(Sprite &sprite, const std::vector<Vector2<int>>& rects, Vector2<unsigned> rectSize) :
			MoveSpriteDescriptor(sprite, sprite, rects, rectSize)
		{
		}

		MoveSpriteDescriptor(Sprite &sprite, Sprite &sprite2, std::vector<std::string> sequences, Vector2<unsigned> rectSize) :
			_isSkill(true),
			_sequences(std::move(sequences)),
			_sprite(sprite),
			_sprite2(sprite2),
			_rectSize(rectSize)
		{
			this->_rects.erase(this->_rects.begin());
		}

		MoveSpriteDescriptor(Sprite &sprite, Sprite &sprite2, std::vector<Vector2<int>> rects, Vector2<unsigned> rectSize) :
			_sprite(sprite),
			_sprite2(sprite2),
			_startRect(rects[0]),
			_rects(std::move(rects)),
			_rectSize(rectSize)
		{
			this->_rects.erase(this->_rects.begin());
		}

		void draw(Vector2<int> pos, Vector2<unsigned> size, bool reverse, SokuLib::Character character, float alpha = 1) const
		{
			this->_sprite2.setSize(size);
			this->_sprite2.tint = DxSokuColor::White * alpha;
			this->_sprite.setSize(size);
			this->_sprite.tint = DxSokuColor::White * alpha;
			if (this->_isSkill)
				return this->_drawSkill(pos, size, reverse, character);
			if (reverse) {
				for (unsigned i = this->_rects.size(); i > 0; i--) {
					this->_sprite.setPosition(pos);
					this->_sprite.rect.top = this->_rects[i - 1].y;
					this->_sprite.rect.left = this->_rects[i - 1].x;
					this->_sprite.rect.width = this->_rectSize.x;
					this->_sprite.rect.height = this->_rectSize.y;
					this->_sprite.draw();
					pos.x -= size.x;
				}
				this->_sprite2.setPosition(pos);
				this->_sprite2.rect.top = this->_startRect.y;
				this->_sprite2.rect.left = this->_startRect.x;
				this->_sprite2.rect.width = this->_rectSize.x;
				this->_sprite2.rect.height = this->_rectSize.y;
				this->_sprite2.draw();
			} else {
				this->_sprite2.setPosition(pos);
				this->_sprite2.rect.top = this->_startRect.y;
				this->_sprite2.rect.left = this->_startRect.x;
				this->_sprite2.rect.width = this->_rectSize.x;
				this->_sprite2.rect.height = this->_rectSize.y;
				this->_sprite2.draw();
				for (const auto &rect : this->_rects) {
					pos.x += size.x;
					this->_sprite.setPosition(pos);
					this->_sprite.rect.top = rect.y;
					this->_sprite.rect.left = rect.x;
					this->_sprite.rect.width = this->_rectSize.x;
					this->_sprite.rect.height = this->_rectSize.y;
					this->_sprite.draw();
				}
			}
		}
	};

	bool operator==(const SokuLib::KeyInput &i1, const SokuLib::KeyInput &i2)
	{
		return !!i1.a == !!i2.a &&
			!!i1.b == !!i2.b &&
			!!i1.c == !!i2.c &&
			!!i1.d == !!i2.d &&
			!!i1.changeCard == !!i2.changeCard &&
			!!i1.spellcard == !!i2.spellcard &&
			(i1.horizontalAxis == 0) == (i2.horizontalAxis == 0) &&
			(i1.horizontalAxis < 0) == (i2.horizontalAxis < 0) &&
			(i1.horizontalAxis > 0) == (i2.horizontalAxis > 0) &&
			(i1.verticalAxis == 0) == (i2.verticalAxis == 0) &&
			(i1.verticalAxis < 0) == (i2.verticalAxis < 0) &&
			(i1.verticalAxis > 0) == (i2.verticalAxis > 0);
	}

	bool operator!=(const SokuLib::KeyInput &i1, const SokuLib::KeyInput &i2)
	{
		return !(i1 == i2);
	}

	static const Vector2<int> dirSheetOffset[9]{
		{32 * 7, 0},
		{32 * 1, 0},
		{32 * 6, 0},
		{32 * 2, 0},
		{   -32, 0},
		{32 * 3, 0},
		{32 * 4, 0},
		{32 * 0, 0},
		{32 * 5, 0},
	};
	static GradiantRect leftBox;
	static GradiantRect rightBox;
	static std::list<Input> leftInputList;
	static std::list<Input> rightInputList;
	static Sprite leftSkillSheet;
	static Sprite rightSkillSheet;
	static Sprite inputSheet;
	static MoveState lastLeftMove;
	static MoveState lastRightMove;
	static const std::map<SokuLib::Action, MoveSpriteDescriptor> moveSprites{
		{ SokuLib::ACTION_5A, {
			inputSheet,
			{A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_f5A, {
			inputSheet,
			{FAR_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_6A, {
			inputSheet,
			{RIGHT_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_2A, {
			inputSheet,
			{DOWN_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_3A, {
			inputSheet,
			{RIGHTDOWN_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_66A, {
			inputSheet,
			{DASH_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j5A, {
			inputSheet,
			{AIR_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j6A, {
			inputSheet,
			{AIR_SPRITE_POS, RIGHT_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j2A, {
			inputSheet,
			{AIR_SPRITE_POS, DOWN_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j8A, {
			inputSheet,
			{AIR_SPRITE_POS, UP_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_f2A, {
			inputSheet,
			{FAR_SPRITE_POS, DOWN_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_5AA, {
			inputSheet,
			{A_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_5AAA, {
			inputSheet,
			{A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_5AAAA, {
			inputSheet,
			{A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS},
 			{24, 32}
		} },
		{ SokuLib::ACTION_5AAAAA, {
			inputSheet,
			{A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS},
  			{24, 32}
		} },
		{ SokuLib::ACTION_j5AA, {
			inputSheet,
			{AIR_SPRITE_POS, A_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_4A, {
			inputSheet,
			{LEFT_SPRITE_POS, A_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_5B, {
			inputSheet,
			{B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_6B, {
			inputSheet,
			{RIGHT_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_2B, {
			inputSheet,
			{DOWN_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j5B, {
			inputSheet,
			{AIR_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j6B, {
			inputSheet,
			{AIR_SPRITE_POS, RIGHT_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j2B, {
			inputSheet,
			{AIR_SPRITE_POS, DOWN_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_4B, {
			inputSheet,
			{LEFT_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_66B, {
			inputSheet,
			{DASH_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j4B, {
			inputSheet,
			{AIR_SPRITE_POS, LEFT_SPRITE_POS, B_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_5C, {
			inputSheet,
			{C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_6C, {
			inputSheet,
			{RIGHT_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_2C, {
			inputSheet,
			{DOWN_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j5C, {
			inputSheet,
			{AIR_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j6C, {
			inputSheet,
			{AIR_SPRITE_POS, RIGHT_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j2C, {
			inputSheet,
			{AIR_SPRITE_POS, DOWN_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_j1C, {
			inputSheet,
			{AIR_SPRITE_POS, LEFTDOWN_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_66C, {
			inputSheet,
			{DASH_SPRITE_POS, C_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_FORWARD_DASH, {
			inputSheet,
			{RIGHT_SPRITE_POS, DASH_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BACKDASH, {
			inputSheet,
			{LEFT_SPRITE_POS, DASH_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_FORWARD_AIRDASH, {
			inputSheet,
			{AIR_SPRITE_POS, RIGHT_SPRITE_POS, DASH_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BACKWARD_AIRDASH, {
			inputSheet,
			{AIR_SPRITE_POS, LEFT_SPRITE_POS, DASH_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_NEUTRAL_HIGH_JUMP, {
			inputSheet,
			{UP_SPRITE_POS, HJ_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_FORWARD_HIGH_JUMP, {
			inputSheet,
			{RIGHTUP_SPRITE_POS, HJ_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BACKWARD_HIGH_JUMP, {
			inputSheet,
			{LEFTUP_SPRITE_POS, HJ_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BE2, {
			inputSheet,
			{DOWN_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BE1, {
			inputSheet,
			{LEFTDOWN_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BE6, {
			inputSheet,
			{RIGHT_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BE4, {
			inputSheet,
			{LEFT_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_jBE4, {
			inputSheet,
			{AIR_SPRITE_POS, LEFT_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_jBE6, {
			inputSheet,
			{AIR_SPRITE_POS, RIGHT_SPRITE_POS, BE_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_FLY, {
			inputSheet,
			{AIR_SPRITE_POS, DASH_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_NEUTRAL_JUMP, {
			inputSheet,
			{UP_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_FORWARD_JUMP, {
			inputSheet,
			{RIGHTUP_SPRITE_POS},
			{24, 32}
		} },
		{ SokuLib::ACTION_BACKWARD_JUMP, {
			inputSheet,
			{LEFTUP_SPRITE_POS},
			{24, 32}
		} }
	};

	void initInputDisplay(LPCSTR profilePath)
	{
		std::string profile = profilePath;

		leftInputList.clear();
		rightInputList.clear();
		leftBox.setPosition({0, 60});
		leftBox.setSize({BOX_WIDTH, BOX_HEIGHT});
		leftBox.fillColors[GradiantRect::RECT_TOP_LEFT_CORNER]    = leftBox.fillColors[GradiantRect::RECT_TOP_RIGHT_CORNER]    = DxSokuColor::Black * MIN_BOX_ALPHA;
		leftBox.fillColors[GradiantRect::RECT_BOTTOM_LEFT_CORNER] = leftBox.fillColors[GradiantRect::RECT_BOTTOM_RIGHT_CORNER] = DxSokuColor::Black * MAX_BOX_ALPHA;
		leftBox.borderColors[0] = leftBox.borderColors[1] = leftBox.borderColors[2] = leftBox.borderColors[3] = DxSokuColor::Transparent;
		rightBox = leftBox;
		rightBox.setPosition({640 - BOX_WIDTH, 60});
		if (!inputSheet.texture.hasTexture())
			Texture::loadFromFile(inputSheet.texture, (profile + "/assets/inputs.png").c_str());
		Texture::loadFromFile(leftSkillSheet.texture, (profile +  + "/assets/skills/" + names[SokuLib::leftChar] + "Skills.png").c_str());
		Texture::loadFromFile(rightSkillSheet.texture, (profile +  + "/assets/skills/" + names[SokuLib::rightChar] + "Skills.png").c_str());
	}

	bool isCancelableByItself(SokuLib::Action action)
	{
		switch (action) {
		case SokuLib::ACTION_4A:
		case SokuLib::ACTION_2A:
		case SokuLib::ACTION_FLY:
		case SokuLib::ACTION_FORWARD_AIRDASH:
		case SokuLib::ACTION_BACKWARD_AIRTECH:
			return true;
		default:
			return false;
		}
	}

	void updateList(std::list<Input> &list, SokuLib::CharacterManager &character, MoveState &last)
	{
		if (list.empty() || list.front().input != character.keyMap) {
			list.push_front({character.keyMap, 1, SokuLib::ACTION_IDLE});
			while (list.size() > 2 * (BOX_HEIGHT / (SPRITE_SIZE + (SPRITE_SIZE / 3))))
				list.pop_back();
		}

		auto realAction = character.objectBase.action;
		auto *front = &list.front();

		if (moveSprites.find(realAction) != moveSprites.end()) {
			if ((realAction != last.action || isCancelableByItself(realAction)) && character.objectBase.frameCount == 0 && character.objectBase.actionBlockId == 0) {
				if (front->action) {
					list.push_front(list.front());
					while (list.size() > 2 * (BOX_HEIGHT / (SPRITE_SIZE + (SPRITE_SIZE / 3))))
						list.pop_back();
					front = &list.front();
					front->duration = 0;
				}
				last.action = front->action = realAction;
			}
			last.animation = character.objectBase.animationCounter;
			last.subFrames = character.objectBase.animationSubFrame;
			last.frames = character.objectBase.frameCount;
		} else
			last.action = SokuLib::ACTION_IDLE;
		front->duration++;
	}

	void updateInputLists()
	{
		updateList(leftInputList,  SokuLib::getBattleMgr().leftCharacterManager,  lastLeftMove);
		updateList(rightInputList, SokuLib::getBattleMgr().rightCharacterManager, lastRightMove);
	}

	void showInput(int value, Vector2<int> &pos, Vector2<int> sheetPos, bool goLeft)
	{
		if (!value)
			return;
		inputSheet.setPosition(pos);
		inputSheet.setSize({SPRITE_SIZE, SPRITE_SIZE});
		inputSheet.rect.top = sheetPos.y;
		inputSheet.rect.left = sheetPos.x;
		inputSheet.rect.width = inputSheet.rect.height = 24;
		inputSheet.draw();
		if (goLeft)
			pos.x -= SPRITE_SIZE;
		else
			pos.x += SPRITE_SIZE;
	}

	void drawInputList(const std::list<Input> &list, Vector2<int> offset, bool reversed, SokuLib::Character character)
	{
		int baseX = offset.x;
		int border = offset.x;
		float alpha = MAX_ALPHA;
		int i = 0;
		SokuLib::KeyInput empty;

		memset(&empty, 0, sizeof(empty));
		if (reversed)
			baseX += BOX_WIDTH - SPRITE_SIZE;
		else
			border += BOX_WIDTH - SPRITE_SIZE;
		offset.y += BOX_HEIGHT + (SPRITE_SIZE / 6);
		for (auto &input : list) {
			if (memcmp(&input.input, &empty, sizeof(empty)) == 0 && !input.action)
				continue;
			offset.y -= SPRITE_SIZE + (SPRITE_SIZE / 3);
			offset.x = baseX;

			int dir = 5 + (input.input.horizontalAxis > 0) - (input.input.horizontalAxis < 0) + ((input.input.verticalAxis < 0) - (input.input.verticalAxis > 0)) * 3;

			inputSheet.tint = DxSokuColor::White * alpha;
			showInput(dir != 5, offset, dirSheetOffset[dir - 1], reversed);
			showInput(input.input.a, offset, {0, 32}, reversed);
			showInput(input.input.b, offset, {32, 32}, reversed);
			showInput(input.input.c, offset, {64, 32}, reversed);
			showInput(input.input.d, offset, {96, 32}, reversed);
			showInput(input.input.changeCard, offset, {128, 32}, reversed);
			showInput(input.input.spellcard, offset, {160, 32}, reversed);

			auto it = moveSprites.find(input.action);

			if (it != moveSprites.end())
				it->second.draw({border, offset.y}, {SPRITE_SIZE - (SPRITE_SIZE / 4), SPRITE_SIZE}, !reversed, character, alpha);
			alpha -= (MAX_ALPHA - MIN_ALPHA) / 10;
			i++;
			if (i >= (BOX_HEIGHT / (SPRITE_SIZE + (SPRITE_SIZE / 3))))
				break;
		}
	}

	void displayInputs()
	{
		if (settings.showLeftInputBox) {
			leftBox.draw();
			drawInputList(leftInputList, {0, 60}, false, SokuLib::leftChar);
		}
		if (settings.showRightInputBox) {
			rightBox.draw();
			drawInputList(rightInputList, {640 - BOX_WIDTH, 60}, true, SokuLib::rightChar);
		}
	}
}