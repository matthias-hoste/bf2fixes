#pragma once

typedef enum
{
	GHTTPFalse,
	GHTTPTrue
} GHTTPBool;

typedef void (*ArrayElementFreeFn)(void* elem);
struct DArrayImplementation
{
	int count, capacity;
	int elemsize;
	int growby;
	ArrayElementFreeFn elemfreefn;
	void* list; //array of elements
};

typedef int GHTTPRequest;

typedef void (*ghttpPostCallback)
(
	GHTTPRequest request,       // The request.
	int bytesPosted,            // The number of bytes of data posted so far.
	int totalBytes,             // The total number of bytes being posted.
	int objectsPosted,          // The total number of data objects uploaded so far.
	int totalObjects,           // The total number of data objects to upload.
	void* param                // User-data.
	);

typedef struct DArrayImplementation* DArray;
typedef struct GHIPost
{
	DArray data;
	ghttpPostCallback callback;
	void* param;
	GHTTPBool hasFiles;
	GHTTPBool hasSoap;
	GHTTPBool useDIME;
	GHTTPBool autoFree;
} GHIPost;
typedef struct GHIPost* GHTTPPost;
typedef enum
{
	GHTTPSocketInit,			// Socket creation and initialization.
	GHTTPHostLookup,            // Resolving hostname to IP (asynchronously if possible).
	GHTTPLookupPending,			// Asychronous DNS lookup pending.
	GHTTPConnecting,            // Waiting for socket connect to complete.
	GHTTPSecuringSession,		// Setup secure channel.
	GHTTPSendingRequest,        // Sending the request.
	GHTTPPosting,               // Positing data (skipped if not posting).
	GHTTPWaiting,               // Waiting for a response.
	GHTTPReceivingStatus,       // Receiving the response status.
	GHTTPReceivingHeaders,      // Receiving the headers.
	GHTTPReceivingFile          // Receiving the file.
} GHTTPState;

typedef __int64  GHTTPByteCount;

typedef enum
{
	GHTTPSuccess,               // 0:  Successfully retrieved file.
	GHTTPOutOfMemory,           // 1:  A memory allocation failed.
	GHTTPBufferOverflow,        // 2:  The user-supplied buffer was too small to hold the file.
	GHTTPParseURLFailed,        // 3:  There was an error parsing the URL.
	GHTTPHostLookupFailed,      // 4:  Failed looking up the hostname.
	GHTTPSocketFailed,          // 5:  Failed to create/initialize/read/write a socket.
	GHTTPConnectFailed,         // 6:  Failed connecting to the http server.
	GHTTPBadResponse,           // 7:  Error understanding a response from the server.
	GHTTPRequestRejected,       // 8:  The request has been rejected by the server.
	GHTTPUnauthorized,          // 9:  Not authorized to get the file.
	GHTTPForbidden,             // 10: The server has refused to send the file.
	GHTTPFileNotFound,          // 11: Failed to find the file on the server.
	GHTTPServerError,           // 12: The server has encountered an internal error.
	GHTTPFileWriteFailed,       // 13: An error occured writing to the local file (for ghttpSaveFile[Ex]).
	GHTTPFileReadFailed,        // 14: There was an error reading from a local file (for posting files from disk).
	GHTTPFileIncomplete,        // 15: Download started but was interrupted.  Only reported if file size is known.
	GHTTPFileToBig,             // 16: The file is to big to be downloaded (size exceeds range of interal data types)
	GHTTPEncryptionError,       // 17: Error with encryption engine.
	GHTTPRequestCancelled       // 18: User requested cancel and/or graceful close.
} GHTTPResult;

typedef void (*ghttpProgressCallback)
(
	GHTTPRequest request,       // The request.
	GHTTPState state,           // The current state of the request.
	const char* buffer,        // The file's bytes so far, NULL if state<GHTTPReceivingFile.
	GHTTPByteCount bufferLen,   // The number of bytes in the buffer, 0 if state<GHTTPReceivingFile.
	GHTTPByteCount bytesReceived, // The total number of bytes receivied, 0 if state<GHTTPReceivingFile.
	GHTTPByteCount totalSize,   // The total size of the file, -1 if unknown.
	void* param                // User-data.
	);

typedef GHTTPBool(*ghttpCompletedCallback)
(
	GHTTPRequest request,       // The request.
	GHTTPResult result,         // The result (success or an error).
	char* buffer,              // The file's bytes (only valid if ghttpGetFile[Ex] was used).
	GHTTPByteCount bufferLen,   // The file's length.
	void* param                // User-data.
	);