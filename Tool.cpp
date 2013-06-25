#include "stdafx.h"
#include "Tool.h"

BOOL Tool::match(TCHAR *fname, TCHAR *filter) {
	// auto trim for ...
	int l = wcslen(filter);
	if (filter[l-2] == 13) filter[l-2]=0, l-=2;
	int i,j=0;
	for (i=0; i<l; i++) {
		if (filter[i] == L'?') {
			j++;
			continue;
		} else if (filter[i] == L'*') {
			int k=1;
			while (TRUE) {
				if (filter[i+k] == 0 && fname[j+k-1] == 0) return TRUE;	// ���� ������ true
				if (fname[j+k-1] == 0) return FALSE;	// ��ǥ ���ڿ��� ���� ������ false
				if (filter[i+k] == L'?' || filter[i+k] == L'*') {	// ���� syntax�� exit
					j += k-1;
					i += k-1;
					break;
				}
				if (filter[i+k] != fname[j+k-1]) {
					j++;
					k=0;
				}
				k++;
			}
		} else {
			if (fname[i]!=filter[j]) return FALSE;
			while (fname[i+1] == L' ')
				i++;
			while (filter[j+1] == L' ')
				j++;
			j++;
		}
	}

	if (fname[i] == 0 && filter[j] == 0) return TRUE;
	else return FALSE;
}