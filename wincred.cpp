// compile on Cygwin terminal
// g++ wincred.cpp -lcredui -o wincred.exe

#include <iostream>
#include <windows.h>
#include <wincred.h>

using namespace std;

void print_usage(char *s) {
	fprintf(stderr, "Usage:\n");
	fprintf(stderr, "\t%s --get target\n", s);
	fprintf(stderr, "\t%s --delete target\n", s);
	fprintf(stderr, "\t%s --read target\n", s);
}

void cred_get(char *target) {
	DWORD dwResult;
	char pszName[CREDUI_MAX_USERNAME_LENGTH + 1] = {0};
	char pszPwd[CREDUI_MAX_PASSWORD_LENGTH + 1] = {0};

	dwResult = CredUIPromptForCredentials(
		NULL,
		target,
		NULL,
		0,
		pszName,
		CREDUI_MAX_USERNAME_LENGTH,
		pszPwd,			
		CREDUI_MAX_PASSWORD_LENGTH,
		NULL,			
		CREDUI_FLAGS_GENERIC_CREDENTIALS | CREDUI_FLAGS_PERSIST);

	switch (dwResult) {
	case ERROR_CANCELLED:
		cerr << "CANCELLED" << endl;
		exit(EXIT_FAILURE);
	case NO_ERROR:
		printf("%s\n", pszName);
		printf("%s\n", pszPwd);
		break;
	default:
		cerr << "ERROR: at " << __LINE__  << ", code is " << dwResult << endl;
		exit(EXIT_FAILURE);
	}
}

void cred_read(char *target) {
	BOOL bRet;
	PCREDENTIAL pCred;
	bRet = CredRead(target, CRED_TYPE_GENERIC, 0, &pCred);
	if (bRet) {
		printf("%s\n", pCred->UserName);
		printf("%ls\n", pCred->CredentialBlob);
		CredFree(pCred);
	} else {
		DWORD error_code = GetLastError();
		if (error_code == ERROR_NOT_FOUND) {
			cerr << "NOT_FOUND: " << target << endl;
		} else {
			cerr << "ERROR: at " << __LINE__  << ", code is " << error_code << endl;
		}
		exit(EXIT_FAILURE);
	}
}

void cred_delete(char *target) {
	BOOL bRet;
	bRet = CredDelete(target, CRED_TYPE_GENERIC, 0);
	if (!bRet) {
		DWORD error_code = GetLastError();
		if (error_code == ERROR_NOT_FOUND) {
			cerr << "NOT_FOUND: " << target << endl;
		} else {
			cerr << "ERROR: at " << __LINE__  << ", code is " << error_code << endl;
			exit(EXIT_FAILURE);
		}
	}
}

int main(int argc, char **argv) {
	if (argc !=3) {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	char *opt = argv[1];
	char *target = argv[2];
	if (strcmp(opt, "--get") == 0) {
		cred_get(target);
	} else if (strcmp(opt, "--delete") == 0) {
		cred_delete(target);
	} else if (strcmp(opt, "--read") == 0) {
		cred_read(target);
	} else {
		print_usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}
