// ������ �� �������
void ProcessCommonDialogEvent(ref NPChar, aref Link, aref NextDiag)
{
    switch (Dialog.CurrentNode)
	{
		case "quests":
			dialog.text = NPCStringReactionRepeat(RandPhraseSimple("����� ������ ������� � �������?", "� ������� ����� ��������� - �������, �������... ��� ��� ������, " + GetAddress_Form(NPChar) + "?"), "�� �������� ���������� � ������� � ������� ���������, " + GetAddress_Form(NPChar) + "...", "������ ��� �� ������� �� ��������� ������ ������...",
                          "���������� ���� � ��� ���� ��������, � ��� ��� � ��, ���������, �������� �� ����...", "block", 1, npchar, Dialog.CurrentNode);
			link.l1 = HeroStringReactionRepeat(RandPhraseSimple("� ���������...", "������ ��� �� � ��� ��������"), "�� ��������...",
                      "�� ��, ������������� � ������ ���...", "���������...", npchar, Dialog.CurrentNode);
			link.l1.go = "exit";
		break;
	}
	UnloadSegment(NPChar.FileDialog2);  // ���� ���-�� ����� ������ switch  �� return �� ������ ������� �����
}

