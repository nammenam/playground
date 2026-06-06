export interface FStopWasm {
    initCamera(width: number, height: number): void;
    updateCamera(dt: number): void;
    onMouseMove(dx: number, dy: number): void;
    onKeyDown(key: number): void;
    onKeyUp(key: number): void;
    getCameraViewMatrix(): number;
    getCameraProjectionMatrix(): number;
    initWorld(): void;
    getSphereInstancesCount(): number;
    getSphereInstanceData(index: number): number;
    memory: WebAssembly.Memory;
}

export async function loadWasm(): Promise<FStopWasm> {
    const response = await fetch("/wasm/main.wasm");
    const buffer = await response.arrayBuffer();
    const { instance } = await WebAssembly.instantiate(buffer, {
        env: {
            // Add any imports Zig might need
        }
    });

    return instance.exports as unknown as FStopWasm;
}

export function getFloat32Array(memory: WebAssembly.Memory, ptr: number, length: number): Float32Array {
    return new Float32Array(memory.buffer, ptr, length);
}
