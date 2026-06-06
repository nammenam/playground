export interface MeshData {
    positions: Float32Array;
    uvs: Float32Array;
    normals: Float32Array;
    indices: Uint32Array;
}

export async function loadObj(url: string): Promise<MeshData> {
    const response = await fetch(url);
    const text = await response.text();

    const positions: number[][] = [];
    const uvs: number[][] = [];
    const normals: number[][] = [];
    
    const vertexData: { p: number, u: number, n: number }[] = [];
    const vertexMap = new Map<string, number>();
    const indices: number[] = [];

    const lines = text.split('\n');
    for (const line of lines) {
        const parts = line.trim().split(/\s+/);
        if (parts[0] === 'v') {
            positions.push(parts.slice(1).map(parseFloat));
        } else if (parts[0] === 'vt') {
            uvs.push(parts.slice(1).map(parseFloat));
        } else if (parts[0] === 'vn') {
            normals.push(parts.slice(1).map(parseFloat));
        } else if (parts[0] === 'f') {
            for (let i = 1; i <= 3; i++) {
                const subparts = parts[i].split('/');
                const key = parts[i];
                if (vertexMap.has(key)) {
                    indices.push(vertexMap.get(key)!);
                } else {
                    const pIdx = parseInt(subparts[0]) - 1;
                    const uIdx = subparts[1] ? parseInt(subparts[1]) - 1 : -1;
                    const nIdx = subparts[2] ? parseInt(subparts[2]) - 1 : -1;
                    
                    const newIdx = vertexData.length;
                    vertexData.push({ p: pIdx, u: uIdx, n: nIdx });
                    vertexMap.set(key, newIdx);
                    indices.push(newIdx);
                }
            }
        }
    }

    const finalPositions = new Float32Array(vertexData.length * 3);
    const finalUvs = new Float32Array(vertexData.length * 2);
    const finalNormals = new Float32Array(vertexData.length * 3);

    for (let i = 0; i < vertexData.length; i++) {
        const { p, u, n } = vertexData[i];
        finalPositions.set(positions[p], i * 3);
        if (u !== -1 && uvs[u]) finalUvs.set(uvs[u], i * 2);
        if (n !== -1 && normals[n]) finalNormals.set(normals[n], i * 3);
    }

    return {
        positions: finalPositions,
        uvs: finalUvs,
        normals: finalNormals,
        indices: new Uint32Array(indices)
    };
}
