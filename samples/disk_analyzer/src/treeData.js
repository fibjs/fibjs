// FILE: treeData.js

const skip_dirs = ["/Volumes", "/System/Volumes"];

const formatSize = (size) => {
    if (size < 1024) return `${size} B`;
    if (size < 1024 * 1024) return `${(size / 1024).toFixed(2)} KB`;
    if (size < 1024 * 1024 * 1024) return `${(size / (1024 * 1024)).toFixed(2)} MB`;
    if (size < 1024 * 1024 * 1024 * 1024) return `${(size / (1024 * 1024 * 1024)).toFixed(2)} GB`;
    return `${(size / (1024 * 1024 * 1024 * 1024)).toFixed(2)} TB`;
};

const formatDate = (date) => {
    const d = new Date(date);
    return d.toISOString().split('T')[0]; // 只保留日期部分
};

function calculateDirectoryDatas(rootNode) {
    let size = 0;
    let contents = 0;
    for (const child of rootNode.children) {
        if (child.isDirectory)
            calculateDirectoryDatas(child);

        size += child.size;
        contents += child.contents;
    }
    rootNode.size = size;
    rootNode.contents = contents;
}

var lastUpdate = 0;
var currentDelayTask = null;

const readDirectory = async (path, updateCallback, rootNode, currentNode) => {
    let entries;
    try {
        entries = await window.app.fs.readdir(path);
    } catch (error) {
        // console.error(`Error reading directory ${path}:`, error);
        return { name: path, size: 0, contents: 0, children: [] };
    }

    const children = currentNode.children;

    for (const entry of entries) {
        const fullPath = `${path === '/' ? '' : path}/${entry}`;
        let stats;
        try {
            stats = await window.app.fs.stat(fullPath);
        } catch (error) {
            // console.error(`Error getting stats for ${fullPath}:`, error);
            continue;
        }

        if (stats.isDirectory) {
            if (!skip_dirs.includes(fullPath)) {
                const childNode = {
                    name: entry,
                    size: 0,
                    modified: formatDate(stats.mtime),
                    contents: 0,
                    children: [],
                    isDirectory: true
                };
                children.push(childNode);
                readDirectory(fullPath, updateCallback, rootNode, childNode);
            }
        } else {
            children.push({
                name: entry,
                size: stats.size,
                modified: formatDate(stats.mtime),
                contents: 1, // 文件计为1个内容
            });
        }
    }

    if (currentDelayTask) {
        clearTimeout(currentDelayTask);
        currentDelayTask = null;
    }

    const now = Date.now();
    if (now - lastUpdate > 300) {
        lastUpdate = now;
        calculateDirectoryDatas(rootNode);
        updateCallback(rootNode);
    } else {
        currentDelayTask = setTimeout(() => {
            lastUpdate = Date.now();
            calculateDirectoryDatas(rootNode);
            updateCallback(rootNode);
        }, 100);
    }
};

const fetchTreeData = async (path, updateCallback) => {
    const rootNode = {
        name: path,
        size: 0,
        contents: 0,
        children: [],
    };
    lastUpdate = 0;
    await readDirectory(path, updateCallback, rootNode, rootNode);
};

const sortTreeData = (node) => {
    if (node.children) {
        node.children.sort((a, b) => b.size - a.size);
        node.children.forEach(sortTreeData);
    }
};

export { formatSize, fetchTreeData, sortTreeData };