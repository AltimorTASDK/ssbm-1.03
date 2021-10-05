#include "hsd/mobj.h"
#include "hsd/tobj.h"
#include "resources/rules/ledge_grab_limit.tex.h"

#include "os/os.h"

// Matanim used to cycle through rule text
extern "C" HSD_MatAnimJoint *MenMainCursorRl_Top_matanim_joint;

void print_matanim(const HSD_MatAnim *matanim, int indent_level)
{
	while (matanim != nullptr) {
		for (auto indent = 0; indent < indent_level; indent++)
			OSReport("    ");
		OSReport("matanim %p\n", matanim);
		const auto *texanim = matanim->texanim;

		for (auto indent = 0; indent < indent_level + 1; indent++)
			OSReport("    ");
		OSReport("Image count: %d\n", texanim->n_imagetbl);
		for  (auto i = 0; i < texanim->n_imagetbl; i++) {
			const auto *image = texanim->imagetbl[i];
			for (auto indent = 0; indent < indent_level + 1; indent++)
				OSReport("    ");
			OSReport("Image %d w %d h %d ptr %p\n", i, image->width, image->height, image->img_ptr);
		}

		for (auto indent = 0; indent < indent_level + 1; indent++)
			OSReport("    ");
		OSReport("Tlut count: %d\n", texanim->n_tluttbl);
		for  (auto i = 0; i < texanim->n_tluttbl; i++) {
			const auto *tlut = texanim->tluttbl[i];
			for (auto indent = 0; indent < indent_level + 1; indent++)
				OSReport("    ");
			OSReport("Tlut %d entries %d name %d fmt %d lut %p", i, tlut->n_entries, tlut->tlut_name, tlut->fmt, tlut->lut);
		}

		matanim = matanim->next;
	}
}

void print_matanimjoint_hierarchy(const HSD_MatAnimJoint *matanimjoint, int indent_level = 0)
{
	while (matanimjoint != nullptr) {
		for (auto indent = 0; indent < indent_level; indent++)
			OSReport("    ");
		OSReport("matanimjoint %p\n", matanimjoint);
		
		if (matanimjoint->matanim != nullptr)
			print_matanim(matanimjoint->matanim, indent_level);

		if (matanimjoint->child != nullptr)
			print_matanimjoint_hierarchy(matanimjoint->child, indent_level + 1);
		
		matanimjoint = matanimjoint->next;
	}
}

extern "C" void orig_Menu_SetupRuleMenu(u32 entry_point);
extern "C" void hook_Menu_SetupRuleMenu(u32 entry_point)
{
	//print_matanimjoint_hierarchy(MenMainCursorRl_Top_matanim_joint);
	print_matanim(MenMainCursorRl_Top_matanim_joint->child->child->next->matanim, 0);

	// Replace rule name textures
	const auto *matanim = MenMainCursorRl_Top_matanim_joint->child->child->next->matanim;
	matanim->texanim->imagetbl[3]->img_ptr = ledge_grab_limit_tex_data;
	OSReport("tex data %p\n", ledge_grab_limit_tex_data);
	
	orig_Menu_SetupRuleMenu(entry_point);
}