
/* "States" to switch off character input (this is lousy, but allows minimal
   restructuring of Stefan's code for event-driven version. Applies only to
   full Windows client														*/

#define	BUSY			0
#define	UNSTARTED		1
#define INIT			10
#define OS_READ			100
#define	WAIT_FOR_KEY	101
#define	TIME_ELAPSE		102
#define	UNDO_EDIT		103
#define CHANGE_FONT		104
#define	RESIZE			105
#define	FILE_OPERATION	106
#define	PALETTE_CHANGE	107
#define	SIGNAL_FINISHED	254
#define	FINISHED		255

