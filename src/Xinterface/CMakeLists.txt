project(xinterface)

add_library(xinterface MODULE 
	InfoHandler.cpp
	InfoHandler.h
	base_video.h
	defines.h
	inode.cpp
	inode.h
	vxservice.h
	xinterface.cpp
	xinterface.h
	xiutil.cpp
	xpictures.h
	xservice.cpp
	xservice.h

	BackScene/backscene.cpp
	BackScene/backscene.h

	HelpChooser/HelpChooser.cpp
	HelpChooser/HelpChooser.h

	Nodes/all_xinode.h
	Nodes/xi_2picture.cpp
	Nodes/xi_2picture.h
	Nodes/xi_VImgScroll.cpp
	Nodes/xi_VImgScroll.h
	Nodes/xi_border.cpp
	Nodes/xi_border.h
	Nodes/xi_bounder.cpp
	Nodes/xi_bounder.h
	Nodes/xi_button.cpp
	Nodes/xi_button.h
	Nodes/xi_changer.cpp
	Nodes/xi_changer.h
	Nodes/xi_checkbuttons.cpp
	Nodes/xi_checkbuttons.h
	Nodes/xi_conthelp.cpp
	Nodes/xi_conthelp.h
	Nodes/xi_editbox.cpp
	Nodes/xi_editbox.h
	Nodes/xi_formttext.cpp
	Nodes/xi_formttext.h
	Nodes/xi_fourimg.cpp
	Nodes/xi_fourimg.h
	Nodes/xi_glowCursor.cpp
	Nodes/xi_glowCursor.h
	Nodes/xi_glower.cpp
	Nodes/xi_glower.h
	Nodes/xi_image.cpp
	Nodes/xi_image.h
	Nodes/xi_imgcollection.cpp
	Nodes/xi_imgcollection.h
	Nodes/xi_keychanger.cpp
	Nodes/xi_keychanger.h
	Nodes/xi_linecollection.cpp
	Nodes/xi_linecollection.h
	Nodes/xi_lrchanger.cpp
	Nodes/xi_lrchanger.h
	Nodes/xi_pceditbox.cpp
	Nodes/xi_pceditbox.h
	Nodes/xi_picture.cpp
	Nodes/xi_picture.h
	Nodes/xi_questtexts.cpp
	Nodes/xi_questtexts.h
	Nodes/xi_questtitles.cpp
	Nodes/xi_questtitles.h
	Nodes/xi_rectangle.cpp
	Nodes/xi_rectangle.h
	Nodes/xi_scrollbar.cpp
	Nodes/xi_scrollbar.h
	Nodes/xi_scrolledpic.cpp
	Nodes/xi_scrolledpic.h
	Nodes/xi_scroller.cpp
	Nodes/xi_scroller.h
	Nodes/xi_scrollimage.cpp
	Nodes/xi_scrollimage.h
	Nodes/xi_scrolltext.cpp
	Nodes/xi_scrolltext.h
	Nodes/xi_slideLine.cpp
	Nodes/xi_slideLine.h
	Nodes/xi_slidepicture.cpp
	Nodes/xi_slidepicture.h
	Nodes/xi_statusline.cpp
	Nodes/xi_statusline.h
	Nodes/xi_strcollection.cpp
	Nodes/xi_strcollection.h
	Nodes/xi_table.cpp
	Nodes/xi_table.h
	Nodes/xi_tabsection.cpp
	Nodes/xi_tabsection.h
	Nodes/xi_textbutton.cpp
	Nodes/xi_textbutton.h
	Nodes/xi_title.cpp
	Nodes/xi_title.h
	Nodes/xi_tooltips.cpp
	Nodes/xi_tooltips.h
	Nodes/xi_util.cpp
	Nodes/xi_util.h
	Nodes/xi_video.cpp
	Nodes/xi_video.h
	Nodes/xi_videorect.cpp
	Nodes/xi_videorect.h
	Nodes/xi_window.cpp
	Nodes/xi_window.h

	QuestFileReader/QuestFileReader.cpp
	QuestFileReader/QuestFileReader.h

	Scrshoter/scrshoter.cpp
	Scrshoter/scrshoter.h

	Stringservice/obj_strservice.cpp
	Stringservice/obj_strservice.h
	Stringservice/strservice.cpp
	Stringservice/strservice.h

	TextureSequence/TextureSequence.cpp
	TextureSequence/TextureSequence.h

	aviplayer/aviplayer.cpp
	aviplayer/aviplayer.h

	editor/defines.cpp
	editor/defines.h
	editor/editor.cpp
	editor/editor.h
	editor/font.cpp
	editor/font.h
	editor/list.cpp
	editor/list.h

	templates/TDynArray.h
	templates/ctext.h
)

target_include_directories(xinterface PRIVATE ${DirectX_INCLUDE_DIR})

target_link_libraries(xinterface PRIVATE ddraw.lib)
target_link_libraries(xinterface PRIVATE amstrmid.lib)