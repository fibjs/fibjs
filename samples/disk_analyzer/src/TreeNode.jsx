// FILE: TreeNode.jsx
import React, { useState, useEffect } from 'react';
import './TreeNode.css'; // 引入样式文件

const calculatePath = (node, parentPath) => {
  return parentPath ? `${parentPath}/${node.name}` : node.name;
};

const formatDate = (dateString) => {
  const date = new Date(dateString);
  return date.toISOString().split('T')[0]; // Extract the date part
};

const TreeNode = ({ node, formatSize, onRefresh, parentPath = '' }) => {
  const [expanded, setExpanded] = useState(node.expanded);
  const [size, setSize] = useState(node.size);
  const [contents, setContents] = useState(node.files);
  const path = calculatePath(node, parentPath);

  const handleToggle = async () => {
    const newExpandedState = !expanded;
    setExpanded(newExpandedState);
    await window.app.setNodeExpanded(path, newExpandedState);
    await onRefresh();
  };

  useEffect(() => {
    setSize(node.size);
    setContents(node.files);
  }, [node]);

  return (
    <div className="tree-node-container">
      <div onClick={handleToggle} className="tree-node">
        <span className="tree-node-icon">
          {node.children && (
            <span>{expanded ? '▼' : '▶'}</span> // 使用 Unicode 字符替代图标
          )}
        </span>
        <span className="tree-node-name">{node.name}</span>
        <div className="tree-node-details">
          <span className="tree-node-size">{formatSize(size)}</span>
          <span className="tree-node-contents">{contents}</span>
          <span className="tree-node-modified">{formatDate(node.modified)}</span>
        </div>
      </div>
      {expanded && node.children && (
        <div style={{ paddingLeft: 20 }}>
          {node.children.map((child, index) => (
            <TreeNode key={index} node={child} formatSize={formatSize} onRefresh={onRefresh} parentPath={path} />
          ))}
        </div>
      )}
    </div>
  );
};

export default TreeNode;