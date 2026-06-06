<script lang="ts">
    import { onMount } from 'svelte';
    import { initWebGPU } from '$lib/webgpu';
    import { loadWasm, getFloat32Array, type FStopWasm } from '$lib/wasm';
    import { loadObj, type MeshData } from '$lib/model';

    let canvas: HTMLCanvasElement;
    let device: GPUDevice;
    let context: GPUCanvasContext;
    let format: GPUTextureFormat;
    let wasm: FStopWasm;

    let pipeline: GPURenderPipeline;
    let cameraBuffer: GPUBuffer;
    let cameraBindGroup: GPUBindGroup;
    let materialBindGroup: GPUBindGroup;
    
    let vertexBuffer: GPUBuffer;
    let uvBuffer: GPUBuffer;
    let normalBuffer: GPUBuffer;
    let indexBuffer: GPUBuffer;
    let instanceBuffer: GPUBuffer;
    
    let meshData: MeshData;
    let numIndices = 0;
    let numInstances = 0;
    let depthTexture: GPUTexture;

    let width = 800;
    let height = 600;

    async function init() {
        const gpu = await initWebGPU(canvas);
        device = gpu.device;
        context = gpu.context!;
        format = gpu.format;

        wasm = await loadWasm();
        wasm.initCamera(width, height);
        wasm.initWorld();

        const shaderRes = await fetch('/shaders/shader.wgsl');
        const shaderCode = await shaderRes.text();
        const shaderModule = device.createShaderModule({ code: shaderCode });

        // Load Mesh
        meshData = await loadObj('/assets/sphere.obj');
        numIndices = meshData.indices.length;

        // Create Buffers
        vertexBuffer = device.createBuffer({
            size: meshData.positions.byteLength,
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
        });
        device.queue.writeBuffer(vertexBuffer, 0, meshData.positions);

        uvBuffer = device.createBuffer({
            size: meshData.uvs.byteLength,
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
        });
        device.queue.writeBuffer(uvBuffer, 0, meshData.uvs);

        normalBuffer = device.createBuffer({
            size: meshData.normals.byteLength,
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
        });
        device.queue.writeBuffer(normalBuffer, 0, meshData.normals);

        indexBuffer = device.createBuffer({
            size: meshData.indices.byteLength,
            usage: GPUBufferUsage.INDEX | GPUBufferUsage.COPY_DST,
        });
        device.queue.writeBuffer(indexBuffer, 0, meshData.indices);

        numInstances = wasm.getSphereInstancesCount();
        instanceBuffer = device.createBuffer({
            size: numInstances * 64, // 16 * 4 bytes
            usage: GPUBufferUsage.VERTEX | GPUBufferUsage.COPY_DST,
        });

        cameraBuffer = device.createBuffer({
            size: 128, // 2 * 16 * 4 bytes
            usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
        });

        depthTexture = device.createTexture({
            size: [width, height],
            format: 'depth24plus',
            usage: GPUTextureUsage.RENDER_ATTACHMENT,
        });

        // Pipeline setup
        const bindGroupLayout0 = device.createBindGroupLayout({
            entries: [{
                binding: 0,
                visibility: GPUShaderStage.VERTEX,
                buffer: { type: 'uniform' }
            }]
        });

        const bindGroupLayout1 = device.createBindGroupLayout({
            entries: [
                { binding: 0, visibility: GPUShaderStage.FRAGMENT, sampler: {} },
                { binding: 1, visibility: GPUShaderStage.FRAGMENT, texture: {} },
                { binding: 2, visibility: GPUShaderStage.FRAGMENT, buffer: { type: 'uniform' } }
            ]
        });

        pipeline = device.createRenderPipeline({
            layout: device.createPipelineLayout({
                bindGroupLayouts: [bindGroupLayout0, bindGroupLayout1]
            }),
            vertex: {
                module: shaderModule,
                entryPoint: 'vs_main',
                buffers: [
                    { // Vertex position (Location 0)
                        arrayStride: 12,
                        attributes: [{ shaderLocation: 0, format: 'float32x3', offset: 0 }]
                    },
                    { // UV (Location 1)
                        arrayStride: 8,
                        attributes: [{ shaderLocation: 1, format: 'float32x2', offset: 0 }]
                    },
                    { // Normal (Location 2)
                        arrayStride: 12,
                        attributes: [{ shaderLocation: 2, format: 'float32x3', offset: 0 }]
                    },
                    { // Instance matrix (Locations 5-8)
                        arrayStride: 64,
                        stepMode: 'instance',
                        attributes: [
                            { shaderLocation: 5, format: 'float32x4', offset: 0 },
                            { shaderLocation: 6, format: 'float32x4', offset: 16 },
                            { shaderLocation: 7, format: 'float32x4', offset: 32 },
                            { shaderLocation: 8, format: 'float32x4', offset: 48 },
                        ]
                    }
                ]
            },
            fragment: {
                module: shaderModule,
                entryPoint: 'fs_main',
                targets: [{ format }]
            },
            primitive: {
                topology: 'triangle-list',
                cullMode: 'back'
            },
            depthStencil: {
                depthWriteEnabled: true,
                depthCompare: 'less',
                format: 'depth24plus'
            }
        });

        cameraBindGroup = device.createBindGroup({
            layout: bindGroupLayout0,
            entries: [{ binding: 0, resource: { buffer: cameraBuffer } }]
        });

        // Placeholder for Material Group
        const sampler = device.createSampler();
        const texture = device.createTexture({
            size: [1, 1],
            format: 'rgba8unorm',
            usage: GPUTextureUsage.TEXTURE_BINDING
        });
        const colorBuffer = device.createBuffer({
            size: 16,
            usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST
        });

        materialBindGroup = device.createBindGroup({
            layout: bindGroupLayout1,
            entries: [
                { binding: 0, resource: sampler },
                { binding: 1, resource: texture.createView() },
                { binding: 2, resource: { buffer: colorBuffer } }
            ]
        });

        requestAnimationFrame(render);
    }

    function render(time: number) {
        if (!wasm || !pipeline) return;

        wasm.updateCamera(0.016);

        // Update Camera Uniforms
        const viewPtr = wasm.getCameraViewMatrix();
        const projPtr = wasm.getCameraProjectionMatrix();
        const viewMatrix = getFloat32Array(wasm.memory, viewPtr, 16);
        const projMatrix = getFloat32Array(wasm.memory, projPtr, 16);
        
        device.queue.writeBuffer(cameraBuffer, 0, viewMatrix);
        device.queue.writeBuffer(cameraBuffer, 64, projMatrix);

        // Update Instance Buffer
        for (let i = 0; i < numInstances; i++) {
            const instPtr = wasm.getSphereInstanceData(i);
            const instMatrix = getFloat32Array(wasm.memory, instPtr, 16);
            device.queue.writeBuffer(instanceBuffer, i * 64, instMatrix);
        }

        const commandEncoder = device.createCommandEncoder();
        const textureView = context.getCurrentTexture().createView();

        const renderPass = commandEncoder.beginRenderPass({
            colorAttachments: [{
                view: textureView,
                clearValue: { r: 0.1, g: 0.2, b: 0.3, a: 1.0 },
                loadOp: 'clear',
                storeOp: 'store'
            }],
            depthStencilAttachment: {
                view: depthTexture.createView(),
                depthClearValue: 1.0,
                depthLoadOp: 'clear',
                depthStoreOp: 'store',
            }
        });

        renderPass.setPipeline(pipeline);
        renderPass.setBindGroup(0, cameraBindGroup);
        renderPass.setBindGroup(1, materialBindGroup);
        renderPass.setVertexBuffer(0, vertexBuffer);
        renderPass.setVertexBuffer(1, uvBuffer);
        renderPass.setVertexBuffer(2, normalBuffer);
        renderPass.setVertexBuffer(3, instanceBuffer);
        renderPass.setIndexBuffer(indexBuffer, 'uint32');
        renderPass.drawIndexed(numIndices, numInstances);

        renderPass.end();
        device.queue.submit([commandEncoder.finish()]);

        requestAnimationFrame(render);
    }

    function handleMouseMove(event: MouseEvent) {
        if (!wasm) return;
        if (document.pointerLockElement === canvas) {
            wasm.onMouseMove(event.movementX, event.movementY);
        }
    }

    function handleKeyDown(event: KeyboardEvent) {
        if (!wasm) return;
        const char = event.key.toUpperCase();
        wasm.onKeyDown(char.charCodeAt(0));
    }

    function handleKeyUp(event: KeyboardEvent) {
        if (!wasm) return;
        const char = event.key.toUpperCase();
        wasm.onKeyUp(char.charCodeAt(0));
    }

    async function togglePointerLock() {
        if (document.pointerLockElement === canvas) {
            document.exitPointerLock();
        } else {
            await canvas.requestPointerLock();
        }
    }

    onMount(() => {
        init();
        window.addEventListener('keydown', handleKeyDown);
        window.addEventListener('keyup', handleKeyUp);
    });
</script>

<style>
    :global(body) {
        margin: 0;
        overflow: hidden;
        background: #000;
    }
    canvas {
        display: block;
        width: 100vw;
        height: 100vh;
    }
</style>

<canvas 
    bind:this={canvas} 
    {width} 
    {height} 
    on:mousemove={handleMouseMove}
    on:click={togglePointerLock}
></canvas>
