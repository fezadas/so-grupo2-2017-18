
#include "../Include/app.h"
#include "../Include/traversedir.h"
#include "cul.h"

HANDLE thread;
long maxNrOfThread = 6;
long actualNrOfThreads = 0;

typedef struct {
	LPCSTR pathOutFiles;	// The root dir of transformed files
	PLIST_ENTRY resultList;	// A list of matched reference file. 
							// Each reference file contains the list of matching transformed files.
	FLIP_enum_t flipType;	// The flip type to match
	INT errorCode;			// 0(OPER_SUCCESS) means the operation concludes successfully
	PCUL cul;

} MUTATIONS_RESULT_CTX, *PMUTATIONS_RESULT_CTX;

// the per reference file context
typedef struct {
	PMUTATIONS_RESULT_CTX global; // pointer to global context
	PFILE_MAP refMap;			  // the  (transformed) reference file mapping
	LIST_ENTRY fileMatchingList;  // the list for identical transformed files
} REFIMAGE_MUTLIST_CTX, *PREFIMAGE_MUTLIST_CTX;


typedef struct {
	FILE_MAP refFileMap;
	FILE_MAP flipFileMap;
	PREFIMAGE_MUTLIST_CTX ctx;
	PMUTATIONS_RESULT_CTX global;
	LPCSTR filePath;
} WORK_ARG_UNIT, *PWORK_ARG_UNIT;




static VOID InitGlobalCtx(PMUTATIONS_RESULT_CTX ctx, LPCSTR pathOutFiles,
	FLIP_enum_t flipType, PLIST_ENTRY result) {
	ctx->pathOutFiles = pathOutFiles;
	ctx->flipType = flipType;
	InitializeListHead(result);
	ctx->resultList = result;
	ctx->errorCode = OPER_SUCCESS; // optimistic initialization
}

static VOID InitRefImageMutListCtx(PREFIMAGE_MUTLIST_CTX ctx, PFILE_MAP refMap, PMUTATIONS_RESULT_CTX global) {
	ctx->refMap = refMap;
	ctx->global = global;
	InitializeListHead(&ctx->fileMatchingList);
}

FORCEINLINE static VOID OperMarkError(PMUTATIONS_RESULT_CTX ctx, INT code) {
	ctx->errorCode = code;
}

FORCEINLINE static BOOL OperHasError(PMUTATIONS_RESULT_CTX ctx) {
	return ctx->errorCode != OPER_SUCCESS;
}


/*
* Called for each found bmp in transformed files dir
* Parameters:
*	filePath - the transformed file path
*	_ctx     - a pointer to the per ref file context structure
**/
BOOL ProcessTransformedFile(LPCSTR filePath, LPVOID _ctx) {
	PREFIMAGE_MUTLIST_CTX ctx = (PREFIMAGE_MUTLIST_CTX)_ctx;
	FILE_MAP fileMap;

	if (!FileMapOpen(&fileMap, filePath)) {
		OperMarkError(ctx->global, OPER_MAP_ERROR);
		return FALSE;
	}

	// Insert filename into list if equals to temporary flip result
	if (fileMap.mapSize == ctx->refMap->mapSize &&
		memcmp(ctx->refMap->hView, fileMap.hView, fileMap.mapSize) == 0) {
		PFILENAME_NODE newNode = (PFILENAME_NODE)malloc(sizeof(FILENAME_NODE));
		newNode->filename = _strdup(filePath);
		InsertTailList(&ctx->fileMatchingList, &newNode->link);
	}
	FileMapClose(&fileMap);
	return TRUE;
}

DWORD WINAPI ThreadProc(LPVOID lpParameter) {
	PWORK_ARG_UNIT args = (PWORK_ARG_UNIT)lpParameter;

	printf("BMPUtils_singlethread.BMP_get_flip_reprodutions_of_file: \n"
		"\tFile to analyse = \"%s\"\n"
		"\tDirectory of files to search for reprodutions = \"%s\"\n"
		"\tFlip type = %s\n", args->filePath, args->global->pathOutFiles,
		args->global->flipType == FLIP_VERTICALLY ? "Flip vertically" : "Flip horizontally");
	// map reference file
	if (!FileMapOpen(&args->refFileMap, args->filePath)) {
		OperMarkError(args->global, OPER_MAP_ERROR);
		return FALSE;
	}

	// do the flip to a temprary map
	if (!FileMapTemp(&args->flipFileMap, (args->refFileMap).mapSize)) {
		OperMarkError(args->global, OPER_MAP_ERROR);
		FileMapClose(&args->refFileMap);
		return FALSE;
	}



	// do the selected flip
	BMP_FlipMem((PUCHAR)(args->refFileMap).hView, (PUCHAR)(args->flipFileMap).hView, args->global->flipType);

	InitRefImageMutListCtx(args->ctx, &args->flipFileMap, args->global);

	BOOL ret = TRUE;

	// Iterate through pathOutFiles directory and sub directories,
	// invoking de processor (ProcessTransformedFile) for each transformed file found
	if (!TraverseDirTree(args->global->pathOutFiles, ".bmp", ProcessTransformedFile, args->ctx)) {
		if (!OperHasError(args->global))
			OperMarkError(args->global, OPER_TRAVERSE_ERROR);
	}
	if (OperHasError(args->global)) {
		ret = FALSE;
		goto terminate;
	}

	if (!IsListEmpty(&(*args->ctx).fileMatchingList)) {
		// Accumulate filename list to final result
		PRES_NODE newNode = (PRES_NODE)malloc(sizeof(RES_NODE));
		newNode->filename = _strdup(args->filePath);
		InitializeListHead(&newNode->files);
		InsertRangeTailList(&newNode->files, &(*(args->ctx)).fileMatchingList);
		InsertTailList(args->global->resultList, &newNode->link);
	}

	CUL_Signal(args->global->cul); // sinalizar fim de trabalho

terminate:
	// Cleanup file resources
	FileMapClose(&args->refFileMap);
	FileMapClose(&args->flipFileMap);
	return ret;


}
/*
* Called for each found bmp in reference files dir
* Parameters:
*	filePath - the reference file path
*	_ctx     - a pointer to the global operation context structure
**/
BOOL BMP_GetFlipsOfRefFile(LPCSTR filePath, LPVOID _ctx) {
	PWORK_ARG_UNIT args = (PWORK_ARG_UNIT)malloc(sizeof(WORK_ARG_UNIT));
	args->global = (PMUTATIONS_RESULT_CTX)malloc(sizeof(MUTATIONS_RESULT_CTX));
	memcpy(args->global, (PMUTATIONS_RESULT_CTX)_ctx, sizeof(MUTATIONS_RESULT_CTX));
	args->filePath = _strdup(filePath);
	args->ctx = (PREFIMAGE_MUTLIST_CTX)malloc(sizeof(REFIMAGE_MUTLIST_CTX));

	CUL_Increment(args->global->cul); // incrementar o contador consoante o numero de files de origem
	if (!QueueUserWorkItem(ThreadProc, args, WT_EXECUTEDEFAULT)) 
		printf("Error");
	return true;
}

/*
*  Recursively traverses the folder with the reference images and
*  find matches in transformed files dir
*	Parameters:BMP
*		pathRefFiles - the reference files directory
*		pathOutFiles - the transformed files directory
*		flipType	 - what transformation to match (FLIP_HORIZONTALLY or FLIP_VERTICALLY)
*		res			 - sentinel node of result list
*/
INT BMP_GetFlipsOfFilesInRefDir(LPCSTR pathRefFiles, LPCSTR pathOutFiles, FLIP_enum_t flipType, PLIST_ENTRY res) {

	MUTATIONS_RESULT_CTX ctx;
	InitGlobalCtx(&ctx, pathOutFiles, flipType, res);
	(&ctx)->cul=(PCUL)malloc(sizeof(CUL));
	CUL_Init((&ctx)->cul,1); // inicializar o sincronizador com o valor 1

	// Iterate through pathRefFiles directory and sub directories
	// invoking de processor (BMP_GetFlipsOfRefFile) for each ref file
	if (!TraverseDirTree(pathRefFiles, ".bmp", BMP_GetFlipsOfRefFile, &ctx)) {
		if (!OperHasError(&ctx))
			OperMarkError(&ctx, OPER_TRAVERSE_ERROR);
	}
	CUL_Signal((&ctx)->cul); // sinalizar fim da thread que deu o trabalho as workers
	CUL_Wait((&ctx)->cul);
	CUL_Destroy((&ctx)->cul);
	return ctx.errorCode;
}