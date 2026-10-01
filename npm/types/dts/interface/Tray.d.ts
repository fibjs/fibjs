/// <reference path="../_import/_fibjs.d.ts" />
/// <reference path="../interface/object.d.ts" />
/// <reference path="../interface/Menu.d.ts" />
/**
 * @description System tray icon, used to display an icon in the system tray
 */
declare class Class_Tray extends Class_object {
    /**
     * @description Queries the menu of the tray icon
     *      @return returns the menu of the tray icon
     *
     */
    getMenu(): Class_Menu;

    /**
     * @description Closes the tray icon
     */
    close(): void;

    close(callback: (err: Error | undefined | null)=>any): void;

    /**
     * @description Closes the tray icon
     */
    closeSync(): void;

    /**
     * @description Closes the tray icon
     */
    closeAsync(): Promise<void>;

}

