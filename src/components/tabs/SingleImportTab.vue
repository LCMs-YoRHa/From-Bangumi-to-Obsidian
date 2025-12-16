<!-- src/components/tabs/SingleImportTab.vue -->
<!-- 单个导入选项卡 -->
<script setup lang="ts">
import { ref, computed } from 'vue';
import { useBangumi } from '@/composables/useBangumi';
import { useTaskStore } from '@/stores/task';
import { IconSingle, IconFolder, IconLoading } from '@/components/icons';

const { exportSingleSubject, fetchSubject, isLoading, error, isAuthenticated } = useBangumi();
const taskStore = useTaskStore();

const subjectId = ref<string>('');
const subjectInfo = ref<{
  name: string;
  nameCn: string;
  score: number | null;
} | null>(null);

const isValidId = computed(() => {
  const id = parseInt(subjectId.value);
  return !isNaN(id) && id > 0;
});

const canStart = computed(() => {
  return isAuthenticated.value && isValidId.value && !isLoading.value;
});

async function handlePreview() {
  if (!isValidId.value) return;

  const id = parseInt(subjectId.value);
  const data = await fetchSubject(id);

  if (data) {
    subjectInfo.value = {
      name: data.subject.name,
      nameCn: data.subject.name_cn || '',
      score: data.subject.rating?.score || null,
    };
  }
}

async function handleExport() {
  if (!isValidId.value) return;

  const id = parseInt(subjectId.value);
  await exportSingleSubject(id);
}
</script>

<template>
  <div class="single-import-tab">
    <div class="tab-header">
      <h3><IconSingle :size="20" class="header-icon" /> 单个导入</h3>
      <p class="tab-description">导出指定的单个条目</p>
    </div>

    <div class="input-section">
      <div class="form-group">
        <label class="form-label">条目 ID</label>
        <div class="input-row">
          <input
            v-model="subjectId"
            type="text"
            class="form-input"
            placeholder="请输入要导出的条目 ID"
            @keyup.enter="handlePreview"
          />
          <button
            class="btn btn-secondary"
            :disabled="!isValidId || isLoading"
            @click="handlePreview"
          >
            预览
          </button>
        </div>
      </div>
    </div>

    <div class="preview-section" v-if="subjectInfo">
      <h4>条目预览</h4>
      <div class="preview-card">
        <div class="preview-item">
          <span class="preview-label">原名:</span>
          <span class="preview-value">{{ subjectInfo.name }}</span>
        </div>
        <div class="preview-item" v-if="subjectInfo.nameCn">
          <span class="preview-label">中文名:</span>
          <span class="preview-value">{{ subjectInfo.nameCn }}</span>
        </div>
        <div class="preview-item" v-if="subjectInfo.score">
          <span class="preview-label">评分:</span>
          <span class="preview-value score">{{ subjectInfo.score }}</span>
        </div>
      </div>
    </div>

    <div class="output-info">
      <IconFolder :size="18" class="info-icon" />
      <span>生成的 Markdown 文件将保存到应用数据目录的 output 文件夹中</span>
    </div>

    <div class="error-message" v-if="error">
      {{ error }}
    </div>

    <div class="result-message" v-if="taskStore.isCompleted">
      {{ taskStore.progress?.message }}
    </div>

    <button
      class="btn btn-primary btn-large"
      :disabled="!canStart"
      @click="handleExport"
    >
      <template v-if="isLoading">
        <IconLoading :size="16" class="btn-icon spinning" /> 处理中...
      </template>
      <template v-else>
        <IconSingle :size="16" class="btn-icon" /> 导出条目
      </template>
    </button>
  </div>
</template>

<style scoped>
.single-import-tab {
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

.btn-icon.spinning {
  animation: spin 1s linear infinite;
}

@keyframes spin {
  from { transform: rotate(0deg); }
  to { transform: rotate(360deg); }
}

.tab-description {
  margin: 0;
  color: #666;
  font-size: 0.9rem;
}

.input-section {
  margin-bottom: 16px;
}

.form-group {
  display: flex;
  flex-direction: column;
}

.form-label {
  margin-bottom: 6px;
  font-size: 0.875rem;
  font-weight: 500;
  color: #555;
}

.input-row {
  display: flex;
  gap: 8px;
}

.form-input {
  flex: 1;
  padding: 10px 12px;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 0.9rem;
}

.form-input:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.preview-section {
  background: #f9f9f9;
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}

.preview-section h4 {
  margin: 0 0 12px;
  font-size: 1rem;
  color: #555;
}

.preview-card {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.preview-item {
  display: flex;
  gap: 8px;
}

.preview-label {
  font-weight: 500;
  color: #666;
  min-width: 60px;
}

.preview-value {
  color: #333;
}

.preview-value.score {
  color: #f59e0b;
  font-weight: 600;
}

.output-info {
  display: flex;
  align-items: center;
  gap: 8px;
  padding: 12px 16px;
  background: rgba(102, 126, 234, 0.1);
  border-radius: 6px;
  color: #667eea;
  font-size: 0.875rem;
  margin-bottom: 16px;
}

.info-icon {
  flex-shrink: 0;
  color: #667eea;
}

.error-message {
  padding: 12px 16px;
  background: #ffebee;
  border-radius: 6px;
  color: #d32f2f;
  font-size: 0.875rem;
  margin-bottom: 16px;
}

.result-message {
  padding: 12px 16px;
  background: #e8f5e9;
  border-radius: 6px;
  color: #388e3c;
  font-size: 0.875rem;
  margin-bottom: 16px;
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

.btn-primary {
  background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
  color: white;
}

.btn-primary:hover:not(:disabled) {
  transform: translateY(-1px);
  box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);
}

.btn-secondary {
  background: #f5f5f5;
  color: #333;
}

.btn-secondary:hover:not(:disabled) {
  background: #e0e0e0;
}

.btn-large {
  width: 100%;
  padding: 14px 24px;
  font-size: 1rem;
}
</style>
