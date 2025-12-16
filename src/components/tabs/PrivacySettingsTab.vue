<!-- src/components/tabs/PrivacySettingsTab.vue -->
<!-- 隐私设置选项卡 -->
<script setup lang="ts">
import { ref, computed } from 'vue';
import { useBangumi } from '@/composables/useBangumi';
import { useCollectionStore } from '@/stores/collection';
import { useTaskStore } from '@/stores/task';
import ProgressIndicator from '@/components/common/ProgressIndicator.vue';
import { IconPrivacy, IconLoading, IconSuccess } from '@/components/icons';
import {
  SubjectType,
  SubjectTypeLabels,
} from '@/types';

const { batchUpdatePrivacy, fetchAllIds, error, isAuthenticated } = useBangumi();
const collectionStore = useCollectionStore();
const taskStore = useTaskStore();

const isPrivate = ref<boolean>(true);
const subjectType = ref<number>(SubjectType.Anime);
const isFetching = ref<boolean>(false);

const subjectTypeOptions = Object.entries(SubjectTypeLabels).map(([value, label]) => ({
  value: Number(value),
  label: `${label} (${value})`,
}));

const hasSubjectIds = computed(() => collectionStore.subjectCount > 0);

const canStart = computed(() => {
  return isAuthenticated.value && hasSubjectIds.value && !taskStore.isRunning;
});

async function handleFetchIds() {
  isFetching.value = true;
  try {
    await fetchAllIds(subjectType.value);
  } finally {
    isFetching.value = false;
  }
}

async function handleUpdatePrivacy() {
  if (!hasSubjectIds.value) return;

  await batchUpdatePrivacy(collectionStore.subjectIds, isPrivate.value);
}
</script>

<template>
  <div class="privacy-settings-tab">
    <div class="tab-header">
      <h3><IconPrivacy :size="20" class="header-icon" /> 隐私设置</h3>
      <p class="tab-description">批量修改收藏条目的隐私状态</p>
    </div>

    <div class="settings-section">
      <h4>第一步：获取条目列表</h4>
      
      <div class="form-row">
        <div class="form-group">
          <label class="form-label">条目类型</label>
          <select v-model="subjectType" class="form-select">
            <option
              v-for="option in subjectTypeOptions"
              :key="option.value"
              :value="option.value"
            >
              {{ option.label }}
            </option>
          </select>
        </div>
        
        <div class="form-group action-group">
          <button
            class="btn btn-secondary"
            :disabled="!isAuthenticated || isFetching"
            @click="handleFetchIds"
          >
            {{ isFetching ? '获取中...' : '获取条目 ID' }}
          </button>
        </div>
      </div>

      <div class="ids-status" v-if="hasSubjectIds">
        <IconSuccess :size="16" class="status-icon" /> 已获取 {{ collectionStore.subjectCount }} 个条目 ID
      </div>
    </div>

    <div class="settings-section">
      <h4>第二步：选择隐私设置</h4>

      <div class="privacy-options">
        <label class="radio-label" :class="{ active: !isPrivate }">
          <input
            v-model="isPrivate"
            type="radio"
            :value="false"
            name="privacy"
          />
          <span class="radio-icon">
            <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
              <circle cx="12" cy="12" r="10"/>
              <line x1="2" y1="12" x2="22" y2="12"/>
              <path d="M12 2a15.3 15.3 0 0 1 4 10 15.3 15.3 0 0 1-4 10 15.3 15.3 0 0 1-4-10 15.3 15.3 0 0 1 4-10z"/>
            </svg>
          </span>
          <span class="radio-text">
            <span class="radio-title">设为公开</span>
            <span class="radio-desc">所有人都可以看到您的收藏</span>
          </span>
        </label>

        <label class="radio-label" :class="{ active: isPrivate }">
          <input
            v-model="isPrivate"
            type="radio"
            :value="true"
            name="privacy"
          />
          <span class="radio-icon"><IconPrivacy :size="24" /></span>
          <span class="radio-text">
            <span class="radio-title">设为私密</span>
            <span class="radio-desc">只有您自己可以看到收藏</span>
          </span>
        </label>
      </div>
    </div>

    <div class="warning-message">
      <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" class="warning-icon">
        <path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/>
        <line x1="12" y1="9" x2="12" y2="13"/>
        <line x1="12" y1="17" x2="12.01" y2="17"/>
      </svg>
      此操作将批量修改所有已获取条目的隐私设置，请谨慎操作
    </div>

    <div class="error-message" v-if="error">
      {{ error }}
    </div>

    <div class="progress-section" v-if="taskStore.isRunning && taskStore.progress">
      <ProgressIndicator
        :current="taskStore.progress.current"
        :total="taskStore.progress.total"
        :message="taskStore.progress.message"
      />
    </div>

    <div class="result-message" v-if="taskStore.isCompleted">
      {{ taskStore.progress?.message }}
    </div>

    <button
      class="btn btn-primary btn-large"
      :disabled="!canStart"
      @click="handleUpdatePrivacy"
    >
      <template v-if="taskStore.isRunning">
        <IconLoading :size="16" class="btn-icon spinning" /> 更新中...
      </template>
      <template v-else>
        <IconPrivacy :size="16" class="btn-icon" /> 更新隐私设置
      </template>
    </button>
  </div>
</template>

<style scoped>
.privacy-settings-tab {
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

.settings-section {
  background: #f9f9f9;
  border-radius: 8px;
  padding: 16px;
  margin-bottom: 16px;
}

.settings-section h4 {
  margin: 0 0 16px;
  font-size: 1rem;
  color: #555;
}

.form-row {
  display: flex;
  gap: 16px;
  align-items: flex-end;
}

.form-group {
  flex: 1;
  display: flex;
  flex-direction: column;
}

.action-group {
  flex: 0;
}

.form-label {
  margin-bottom: 6px;
  font-size: 0.875rem;
  font-weight: 500;
  color: #555;
}

.form-select {
  padding: 10px 12px;
  border: 1px solid #ddd;
  border-radius: 6px;
  font-size: 0.9rem;
  background: white;
}

.form-select:focus {
  outline: none;
  border-color: #667eea;
  box-shadow: 0 0 0 3px rgba(102, 126, 234, 0.1);
}

.ids-status {
  margin-top: 12px;
  padding: 8px 12px;
  background: #e8f5e9;
  border-radius: 6px;
  color: #388e3c;
  font-size: 0.875rem;
  display: flex;
  align-items: center;
  gap: 6px;
}

.ids-status .status-icon {
  color: #388e3c;
}

.privacy-options {
  display: flex;
  flex-direction: column;
  gap: 12px;
}

.radio-label {
  display: flex;
  align-items: center;
  gap: 12px;
  padding: 16px;
  background: white;
  border: 2px solid #e0e0e0;
  border-radius: 8px;
  cursor: pointer;
  transition: all 0.2s;
}

.radio-label:hover {
  border-color: #bdbdbd;
}

.radio-label.active {
  border-color: #667eea;
  background: rgba(102, 126, 234, 0.05);
}

.radio-label input {
  display: none;
}

.radio-icon {
  color: #667eea;
  display: flex;
  align-items: center;
  justify-content: center;
}

.radio-text {
  display: flex;
  flex-direction: column;
  gap: 2px;
}

.radio-title {
  font-weight: 500;
  color: #333;
}

.radio-desc {
  font-size: 0.8rem;
  color: #666;
}

.warning-message {
  padding: 12px 16px;
  background: #fff3e0;
  border-radius: 6px;
  color: #e65100;
  font-size: 0.875rem;
  margin-bottom: 16px;
  display: flex;
  align-items: center;
  gap: 8px;
}

.warning-icon {
  flex-shrink: 0;
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

.progress-section {
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
  white-space: nowrap;
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
