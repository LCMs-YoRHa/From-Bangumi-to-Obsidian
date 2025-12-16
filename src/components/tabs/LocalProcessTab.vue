<!-- src/components/tabs/LocalProcessTab.vue -->
<!-- 本地处理选项卡 -->
<script setup lang="ts">
import { ref, onMounted } from 'vue';
import { useFileSystem } from '@/composables/useFileSystem';
import { useCollectionStore } from '@/stores/collection';
import { useTaskStore } from '@/stores/task';
import ProgressIndicator from '@/components/common/ProgressIndicator.vue';
import { IconSettings, IconFolder, IconRefresh } from '@/components/icons';

const { listOutputFiles, openOutputDir, outputFiles, formatFileSize, formatTimestamp } = useFileSystem();
const collectionStore = useCollectionStore();
const taskStore = useTaskStore();

const fileCount = ref(0);
const isRefreshing = ref(false);

async function refreshFileStatus() {
  isRefreshing.value = true;
  try {
    await listOutputFiles();
    fileCount.value = outputFiles.value.length;
  } catch (e) {
    console.error('刷新文件状态失败:', e);
  } finally {
    isRefreshing.value = false;
  }
}

async function handleOpenOutputDir() {
  try {
    await openOutputDir();
  } catch (e) {
    console.error('打开目录失败:', e);
  }
}

onMounted(() => {
  refreshFileStatus();
});
</script>

<template>
  <div class="local-process-tab">
    <div class="tab-header">
      <h3><IconSettings :size="20" class="header-icon" /> 本地管理</h3>
      <p class="tab-description">管理已导出的 Markdown 文件</p>
    </div>

    <div class="status-section">
      <div class="status-card">
        <div class="status-icon"><IconFolder :size="32" /></div>
        <div class="status-info">
          <span class="status-label">已导出文件</span>
          <span class="status-value">{{ fileCount }} 个</span>
        </div>
      </div>

      <div class="status-card">
        <div class="status-icon"><IconRefresh :size="32" /></div>
        <div class="status-info">
          <span class="status-label">缓存条目 ID</span>
          <span class="status-value">{{ collectionStore.subjectCount }} 个</span>
        </div>
      </div>
    </div>

    <div class="actions-section">
      <button
        class="btn btn-secondary"
        :disabled="isRefreshing"
        @click="refreshFileStatus"
      >
        <template v-if="isRefreshing">刷新中...</template>
        <template v-else><IconRefresh :size="14" class="btn-icon" /> 刷新状态</template>
      </button>
      <button class="btn btn-secondary" @click="handleOpenOutputDir">
        <IconFolder :size="14" class="btn-icon" /> 打开输出目录
      </button>
    </div>

    <div class="files-section" v-if="outputFiles.length > 0">
      <h4>最近导出的文件</h4>
      <div class="file-list">
        <div
          v-for="file in outputFiles.slice(0, 10)"
          :key="file.path"
          class="file-item"
        >
          <div class="file-info">
            <span class="file-name">{{ file.name }}</span>
            <span class="file-meta">
              {{ formatFileSize(file.size) }} · {{ formatTimestamp(file.modified) }}
            </span>
          </div>
        </div>
      </div>
      <p v-if="outputFiles.length > 10" class="more-files">
        还有 {{ outputFiles.length - 10 }} 个文件...
      </p>
    </div>

    <div class="empty-state" v-else>
      <div class="empty-icon"><IconFolder :size="48" /></div>
      <p>暂无导出文件</p>
      <p class="empty-hint">使用"批量导入"或"单个导入"功能生成 Markdown 文件</p>
    </div>

    <div class="progress-section" v-if="taskStore.isRunning && taskStore.progress">
      <ProgressIndicator
        :current="taskStore.progress.current"
        :total="taskStore.progress.total"
        :message="taskStore.progress.message"
      />
    </div>
  </div>
</template>

<style scoped>
.local-process-tab {
  padding: 20px;
}

.tab-header {
  margin-bottom: 24px;
}

.tab-header h3 {
  margin: 0 0 8px;
  font-size: 1.25rem;
  color: #333;
  display: flex;
  align-items: center;
  gap: 8px;
}

.header-icon {
  color: #667eea;
}

.btn-icon {
  vertical-align: middle;
}

.tab-description {
  margin: 0;
  color: #666;
  font-size: 0.9rem;
}

.status-section {
  display: grid;
  grid-template-columns: repeat(auto-fit, minmax(180px, 1fr));
  gap: 16px;
  margin-bottom: 16px;
}

.status-card {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 16px;
  background: #f9f9f9;
  border-radius: 8px;
}

.status-icon {
  color: #667eea;
  display: flex;
  align-items: center;
  justify-content: center;
}

.status-info {
  display: flex;
  flex-direction: column;
}

.status-label {
  font-size: 0.875rem;
  color: #666;
}

.status-value {
  font-size: 1.25rem;
  font-weight: 600;
  color: #333;
}

.actions-section {
  display: flex;
  gap: 8px;
  margin-bottom: 24px;
}

.files-section {
  margin-bottom: 16px;
}

.files-section h4 {
  margin: 0 0 12px;
  font-size: 1rem;
  color: #555;
}

.file-list {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.file-item {
  display: flex;
  justify-content: space-between;
  align-items: center;
  padding: 12px 16px;
  background: #f9f9f9;
  border-radius: 6px;
}

.file-info {
  display: flex;
  flex-direction: column;
  gap: 4px;
}

.file-name {
  font-weight: 500;
  color: #333;
}

.file-meta {
  font-size: 0.75rem;
  color: #999;
}

.more-files {
  text-align: center;
  color: #666;
  font-size: 0.875rem;
  margin-top: 8px;
}

.empty-state {
  text-align: center;
  padding: 40px 20px;
  color: #999;
}

.empty-icon {
  color: #ccc;
  margin-bottom: 16px;
  display: flex;
  justify-content: center;
}

.empty-state p {
  margin: 0 0 8px;
}

.empty-hint {
  font-size: 0.875rem;
}

.progress-section {
  margin-top: 16px;
}

.btn {
  padding: 10px 20px;
  border-radius: 6px;
  font-size: 0.9rem;
  font-weight: 500;
  cursor: pointer;
  transition: all 0.2s;
  border: none;
}

.btn:disabled {
  opacity: 0.6;
  cursor: not-allowed;
}

.btn-secondary {
  background: #f5f5f5;
  color: #333;
}

.btn-secondary:hover:not(:disabled) {
  background: #e0e0e0;
}
</style>
